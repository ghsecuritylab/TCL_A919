

#ifndef _FC_LIBFC_H_
#define _FC_LIBFC_H_

#define FC_LIBFC_LOGGING 0x01 /* General logging, not categorized */
#define FC_LPORT_LOGGING 0x02 /* lport layer logging */
#define FC_DISC_LOGGING	 0x04 /* discovery layer logging */
#define FC_RPORT_LOGGING 0x08 /* rport layer logging */
#define FC_FCP_LOGGING	 0x10 /* I/O path logging */
#define FC_EM_LOGGING	 0x20 /* Exchange Manager logging */
#define FC_EXCH_LOGGING	 0x40 /* Exchange/Sequence logging */
#define FC_SCSI_LOGGING	 0x80 /* SCSI logging (mostly error handling) */

extern unsigned int fc_debug_logging;

#define FC_CHECK_LOGGING(LEVEL, CMD)			\
	do {						\
		if (unlikely(fc_debug_logging & LEVEL))	\
			do {				\
				CMD;			\
			} while (0);			\
	} while (0)

#define FC_LIBFC_DBG(fmt, args...)					\
	FC_CHECK_LOGGING(FC_LIBFC_LOGGING,				\
			 printk(KERN_INFO "libfc: " fmt, ##args))

#define FC_LPORT_DBG(lport, fmt, args...)				\
	FC_CHECK_LOGGING(FC_LPORT_LOGGING,				\
			 printk(KERN_INFO "host%u: lport %6.6x: " fmt,	\
				(lport)->host->host_no,			\
				(lport)->port_id, ##args))

#define FC_DISC_DBG(disc, fmt, args...)				\
	FC_CHECK_LOGGING(FC_DISC_LOGGING,			\
			 printk(KERN_INFO "host%u: disc: " fmt,	\
				(disc)->lport->host->host_no,	\
				##args))

#define FC_RPORT_ID_DBG(lport, port_id, fmt, args...)			\
	FC_CHECK_LOGGING(FC_RPORT_LOGGING,				\
			 printk(KERN_INFO "host%u: rport %6.6x: " fmt,	\
				(lport)->host->host_no,			\
				(port_id), ##args))

#define FC_RPORT_DBG(rdata, fmt, args...)				\
	FC_RPORT_ID_DBG((rdata)->local_port, (rdata)->ids.port_id, fmt, ##args)

#define FC_FCP_DBG(pkt, fmt, args...)					\
	FC_CHECK_LOGGING(FC_FCP_LOGGING,				\
			 printk(KERN_INFO "host%u: fcp: %6.6x: " fmt,	\
				(pkt)->lp->host->host_no,		\
				pkt->rport->port_id, ##args))

#define FC_EXCH_DBG(exch, fmt, args...)					\
	FC_CHECK_LOGGING(FC_EXCH_LOGGING,				\
			 printk(KERN_INFO "host%u: xid %4x: " fmt,	\
				(exch)->lp->host->host_no,		\
				exch->xid, ##args))

#define FC_SCSI_DBG(lport, fmt, args...)				\
	FC_CHECK_LOGGING(FC_SCSI_LOGGING,				\
			 printk(KERN_INFO "host%u: scsi: " fmt,		\
				(lport)->host->host_no,	##args))

void fc_fcp_ddp_setup(struct fc_fcp_pkt *fsp, u16 xid);

int fc_setup_exch_mgr(void);
void fc_destroy_exch_mgr(void);
int fc_setup_rport(void);
void fc_destroy_rport(void);
int fc_setup_fcp(void);
void fc_destroy_fcp(void);

const char *fc_els_resp_type(struct fc_frame *);

u32 fc_copy_buffer_to_sglist(void *buf, size_t len,
			     struct scatterlist *sg,
			     u32 *nents, size_t *offset,
			     enum km_type km_type, u32 *crc);

#endif /* _FC_LIBFC_H_ */
