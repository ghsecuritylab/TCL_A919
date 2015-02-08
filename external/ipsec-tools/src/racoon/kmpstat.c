/*	$NetBSD: kmpstat.c,v 1.4.6.2 2007/11/06 16:41:33 vanhu Exp $	*/

/*	$KAME: kmpstat.c,v 1.33 2004/08/16 08:20:28 itojun Exp $	*/


#include "config.h"

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/pfkeyv2.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif
#include <netdb.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <err.h>
#include <sys/ioctl.h> 
#include <resolv.h>

#include "libpfkey.h"

#include "var.h"
#include "misc.h"
#include "vmbuf.h"
#include "plog.h"
#include "debug.h"
#include "sockmisc.h"

#include "racoonctl.h"
#include "admin.h"
#include "schedule.h"
#include "isakmp_var.h"
#include "isakmp.h"
#include "isakmp_xauth.h"
#include "isakmp_var.h"
#include "isakmp_cfg.h"
#include "oakley.h"
#include "handler.h"
#include "pfkey.h"
#include "admin.h"
#include "evt.h"
#include "admin_var.h"
#include "ipsec_doi.h"

u_int32_t racoonctl_interface = RACOONCTL_INTERFACE;
u_int32_t racoonctl_interface_major = RACOONCTL_INTERFACE_MAJOR;

static int so;
u_int32_t loglevel = 0;

int
com_init()
{
	struct sockaddr_un name;

	memset(&name, 0, sizeof(name));
	name.sun_family = AF_UNIX;
	snprintf(name.sun_path, sizeof(name.sun_path),
		"%s", adminsock_path);

	so = socket(AF_UNIX, SOCK_STREAM, 0);
	if (so < 0)
		return -1;

	if (connect(so, (struct sockaddr *)&name, sizeof(name)) < 0) {
		(void)close(so);
		return -1;
	}

	return 0;
}

int
com_send(combuf)
	vchar_t *combuf;
{
	int len;

	if ((len = send(so, combuf->v, combuf->l, 0)) == -1) {
		perror("send");
		(void)close(so);
		return -1;
	}

	return 0;
}

int
com_recv(combufp) 
	vchar_t **combufp;
{
	struct admin_com h, *com;
	caddr_t buf;
	int len;
	int l = 0;
	caddr_t p;

	if (combufp == NULL)
		return -1;

	/* receive by PEEK */ 
	if ((len = recv(so, &h, sizeof(h), MSG_PEEK)) == -1)
		goto bad1;

	/* sanity check */
	if (len < sizeof(h))
		goto bad1;

	if (h.ac_errno) {
		errno = h.ac_errno;
		goto bad1;
	}

	/* allocate buffer */
	if ((*combufp = vmalloc(h.ac_len)) == NULL)
		goto bad1;

	/* read real message */
	p = (*combufp)->v;
	while (l < len) {
		if ((len = recv(so, p, h.ac_len, 0)) < 0) {
			perror("recv");
			goto bad2;
		}
		l += len;
		p += len;
	}
	
	return 0;

bad2:
	vfree(*combufp);
bad1:
	*combufp = NULL;
	return -1;
}

void
_plog(int pri, const char *func, struct sockaddr *sa, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
}

void
plogdump(pri, data, len) 
	int pri;
	void *data;
	size_t len;
{
	return;
}

struct sockaddr *
get_sockaddr(family, name, port)
	int family;
	char *name, *port;
{
	struct addrinfo hint, *ai;
	int error;

	memset(&hint, 0, sizeof(hint));
	hint.ai_family = PF_UNSPEC;
	hint.ai_family = family;
	hint.ai_socktype = SOCK_STREAM;

	error = getaddrinfo(name, port, &hint, &ai);
	if (error != 0) {
		printf("%s: %s/%s\n", gai_strerror(error), name, port);
		return NULL;
	}

	return ai->ai_addr;
}
