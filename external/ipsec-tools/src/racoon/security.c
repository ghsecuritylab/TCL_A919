

#include "config.h"

#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <selinux/selinux.h>
#include <selinux/flask.h>
#include <selinux/av_permissions.h>
#include <selinux/avc.h>
#include <selinux/context.h>

#include "var.h"
#include "vmbuf.h"
#include "misc.h"
#include "plog.h"

#include "isakmp_var.h"
#include "isakmp.h"
#include "ipsec_doi.h"
#include "policy.h"
#include "proposal.h"
#include "strnames.h"
#include "handler.h"

int
get_security_context(sa, p)
	vchar_t *sa;
	struct policyindex *p;
{
	int len = 0;
	int flag, type = 0;
	u_int16_t lorv;
	caddr_t bp;
	vchar_t *pbuf = NULL;
	vchar_t *tbuf = NULL;
	struct isakmp_parse_t *pa;
	struct isakmp_parse_t *ta;
	struct isakmp_pl_p *prop;
	struct isakmp_pl_t *trns;
	struct isakmp_data *d;
	struct ipsecdoi_sa_b *sab = (struct ipsecdoi_sa_b *)sa->v;
	
	/* check SA payload size */
	if (sa->l < sizeof(*sab)) {
		plog(LLV_ERROR, LOCATION, NULL,
			"Invalid SA length = %zu.\n", sa->l);
		return -1;
	}

	bp = (caddr_t)(sab + 1); /* here bp points to first proposal payload */
	len = sa->l - sizeof(*sab);

	pbuf = isakmp_parsewoh(ISAKMP_NPTYPE_P, (struct isakmp_gen *)bp, len);
	if (pbuf == NULL)
		return -1;

	pa = (struct isakmp_parse_t *)pbuf->v; 
        /* check the value of next payload */
	if (pa->type != ISAKMP_NPTYPE_P) {
		plog(LLV_ERROR, LOCATION, NULL,
			"Invalid payload type=%u\n", pa->type);
		vfree(pbuf);
		return -1;
	}

	if (pa->len == 0) {
		plog(LLV_ERROR, LOCATION, NULL,
		"invalid proposal with length %d\n", pa->len);
		vfree(pbuf);
		return -1;
	}

	/* our first proposal */
	prop = (struct isakmp_pl_p *)pa->ptr;

	/* now get transform */
	bp = (caddr_t)prop + sizeof(struct isakmp_pl_p) + prop->spi_size;
	len = ntohs(prop->h.len) - 
		(sizeof(struct isakmp_pl_p) + prop->spi_size);
	tbuf = isakmp_parsewoh(ISAKMP_NPTYPE_T, (struct isakmp_gen *)bp, len);
	if (tbuf == NULL)
		return -1;

	ta = (struct isakmp_parse_t *)tbuf->v;
	if (ta->type != ISAKMP_NPTYPE_T) {
		plog(LLV_ERROR, LOCATION, NULL,
		     "Invalid payload type=%u\n", ta->type);
		return -1;
	}
	
	trns = (struct isakmp_pl_t *)ta->ptr;

	len = ntohs(trns->h.len) - sizeof(struct isakmp_pl_t);
	d = (struct isakmp_data *)((caddr_t)trns + sizeof(struct isakmp_pl_t));

	while (len > 0) {
		type = ntohs(d->type) & ~ISAKMP_GEN_MASK;
		flag = ntohs(d->type) & ISAKMP_GEN_MASK;
		lorv = ntohs(d->lorv);

		if (type != IPSECDOI_ATTR_SECCTX) {
			if (flag) {
				len -= sizeof(*d);
				d = (struct isakmp_data *)((char *)d 
				     + sizeof(*d));
			} else {
				len -= (sizeof(*d) + lorv);
				d = (struct isakmp_data *)((caddr_t)d
				     + sizeof(*d) + lorv);
			}
		} else {
			flag = ntohs(d->type & ISAKMP_GEN_MASK);
			if (flag) {
				plog(LLV_ERROR, LOCATION, NULL,
				     "SECCTX must be in TLV.\n");
				return -1;
			}
			memcpy(&p->sec_ctx, d + 1, lorv);
			p->sec_ctx.ctx_strlen = ntohs(p->sec_ctx.ctx_strlen);
			return 0;
		}
	}
	return 0;
}

void
set_secctx_in_proposal(iph2, spidx)
	struct ph2handle *iph2;
	struct policyindex spidx;
{
	iph2->proposal->sctx.ctx_doi = spidx.sec_ctx.ctx_doi;
	iph2->proposal->sctx.ctx_alg = spidx.sec_ctx.ctx_alg;
	iph2->proposal->sctx.ctx_strlen = spidx.sec_ctx.ctx_strlen;
		memcpy(iph2->proposal->sctx.ctx_str, spidx.sec_ctx.ctx_str,
			spidx.sec_ctx.ctx_strlen);
}



static int mls_ready = 0;

void
init_avc(void)
{
	if (!is_selinux_mls_enabled()) {
		plog(LLV_ERROR, LOCATION, NULL, "racoon: MLS support is not"
				" enabled.\n");
		return;
	}

	if (avc_init("racoon", NULL, NULL, NULL, NULL) == 0)
		mls_ready = 1;
	else
		plog(LLV_ERROR, LOCATION, NULL, 
		     "racoon: could not initialize avc.\n");
}


int
within_range(security_context_t sl, security_context_t range)
{
	int rtn = 1;
	security_id_t slsid;
	security_id_t rangesid;
	struct av_decision avd;
	security_class_t tclass;
	access_vector_t av;

	if (!*range)	/* This policy doesn't have security context */
		return 1;

	if (!mls_ready)  /* mls may not be enabled */
		return 0;

	/*
	 * Get the sids for the sl and range contexts
	 */
	rtn = avc_context_to_sid(sl, &slsid);
	if (rtn != 0) {
		plog(LLV_ERROR, LOCATION, NULL, 
				"within_range: Unable to retrieve "
				"sid for sl context (%s).\n", sl);
		return 0;
	}
	rtn = avc_context_to_sid(range, &rangesid);
	if (rtn != 0) {
		plog(LLV_ERROR, LOCATION, NULL, 
				"within_range: Unable to retrieve "
				"sid for range context (%s).\n", range);
		sidput(slsid);
		return 0;
	}

	/* 
	 * Straight up test between sl and range
	 */
	tclass = SECCLASS_ASSOCIATION;
	av = ASSOCIATION__POLMATCH;
	rtn = avc_has_perm(slsid, rangesid, tclass, av, NULL, &avd);
	if (rtn != 0) {
		plog(LLV_INFO, LOCATION, NULL, 
			"within_range: The sl is not within range\n");
		sidput(slsid);
		sidput(rangesid);
		return 0;
	}
	plog(LLV_DEBUG, LOCATION, NULL, 
		"within_range: The sl (%s) is within range (%s)\n", sl, range);
		return 1;
}
