

#include <linux/types.h>
#include <linux/kernel.h>

#include <linux/delay.h>
#include <linux/init.h>
#include <linux/rio.h>
#include <linux/rio_drv.h>
#include <linux/rio_ids.h>
#include <linux/rio_regs.h>
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/interrupt.h>

#include "rio.h"

static LIST_HEAD(rio_mports);

u16 rio_local_get_device_id(struct rio_mport *port)
{
	u32 result;

	rio_local_read_config_32(port, RIO_DID_CSR, &result);

	return (RIO_GET_DID(port->sys_size, result));
}

int rio_request_inb_mbox(struct rio_mport *mport,
			 void *dev_id,
			 int mbox,
			 int entries,
			 void (*minb) (struct rio_mport * mport, void *dev_id, int mbox,
				       int slot))
{
	int rc = 0;

	struct resource *res = kmalloc(sizeof(struct resource), GFP_KERNEL);

	if (res) {
		rio_init_mbox_res(res, mbox, mbox);

		/* Make sure this mailbox isn't in use */
		if ((rc =
		     request_resource(&mport->riores[RIO_INB_MBOX_RESOURCE],
				      res)) < 0) {
			kfree(res);
			goto out;
		}

		mport->inb_msg[mbox].res = res;

		/* Hook the inbound message callback */
		mport->inb_msg[mbox].mcback = minb;

		rc = rio_open_inb_mbox(mport, dev_id, mbox, entries);
	} else
		rc = -ENOMEM;

      out:
	return rc;
}

int rio_release_inb_mbox(struct rio_mport *mport, int mbox)
{
	rio_close_inb_mbox(mport, mbox);

	/* Release the mailbox resource */
	return release_resource(mport->inb_msg[mbox].res);
}

int rio_request_outb_mbox(struct rio_mport *mport,
			  void *dev_id,
			  int mbox,
			  int entries,
			  void (*moutb) (struct rio_mport * mport, void *dev_id, int mbox, int slot))
{
	int rc = 0;

	struct resource *res = kmalloc(sizeof(struct resource), GFP_KERNEL);

	if (res) {
		rio_init_mbox_res(res, mbox, mbox);

		/* Make sure this outbound mailbox isn't in use */
		if ((rc =
		     request_resource(&mport->riores[RIO_OUTB_MBOX_RESOURCE],
				      res)) < 0) {
			kfree(res);
			goto out;
		}

		mport->outb_msg[mbox].res = res;

		/* Hook the inbound message callback */
		mport->outb_msg[mbox].mcback = moutb;

		rc = rio_open_outb_mbox(mport, dev_id, mbox, entries);
	} else
		rc = -ENOMEM;

      out:
	return rc;
}

int rio_release_outb_mbox(struct rio_mport *mport, int mbox)
{
	rio_close_outb_mbox(mport, mbox);

	/* Release the mailbox resource */
	return release_resource(mport->outb_msg[mbox].res);
}

static int
rio_setup_inb_dbell(struct rio_mport *mport, void *dev_id, struct resource *res,
		    void (*dinb) (struct rio_mport * mport, void *dev_id, u16 src, u16 dst,
				  u16 info))
{
	int rc = 0;
	struct rio_dbell *dbell;

	if (!(dbell = kmalloc(sizeof(struct rio_dbell), GFP_KERNEL))) {
		rc = -ENOMEM;
		goto out;
	}

	dbell->res = res;
	dbell->dinb = dinb;
	dbell->dev_id = dev_id;

	list_add_tail(&dbell->node, &mport->dbells);

      out:
	return rc;
}

int rio_request_inb_dbell(struct rio_mport *mport,
			  void *dev_id,
			  u16 start,
			  u16 end,
			  void (*dinb) (struct rio_mport * mport, void *dev_id, u16 src,
					u16 dst, u16 info))
{
	int rc = 0;

	struct resource *res = kmalloc(sizeof(struct resource), GFP_KERNEL);

	if (res) {
		rio_init_dbell_res(res, start, end);

		/* Make sure these doorbells aren't in use */
		if ((rc =
		     request_resource(&mport->riores[RIO_DOORBELL_RESOURCE],
				      res)) < 0) {
			kfree(res);
			goto out;
		}

		/* Hook the doorbell callback */
		rc = rio_setup_inb_dbell(mport, dev_id, res, dinb);
	} else
		rc = -ENOMEM;

      out:
	return rc;
}

int rio_release_inb_dbell(struct rio_mport *mport, u16 start, u16 end)
{
	int rc = 0, found = 0;
	struct rio_dbell *dbell;

	list_for_each_entry(dbell, &mport->dbells, node) {
		if ((dbell->res->start == start) && (dbell->res->end == end)) {
			found = 1;
			break;
		}
	}

	/* If we can't find an exact match, fail */
	if (!found) {
		rc = -EINVAL;
		goto out;
	}

	/* Delete from list */
	list_del(&dbell->node);

	/* Release the doorbell resource */
	rc = release_resource(dbell->res);

	/* Free the doorbell event */
	kfree(dbell);

      out:
	return rc;
}

struct resource *rio_request_outb_dbell(struct rio_dev *rdev, u16 start,
					u16 end)
{
	struct resource *res = kmalloc(sizeof(struct resource), GFP_KERNEL);

	if (res) {
		rio_init_dbell_res(res, start, end);

		/* Make sure these doorbells aren't in use */
		if (request_resource(&rdev->riores[RIO_DOORBELL_RESOURCE], res)
		    < 0) {
			kfree(res);
			res = NULL;
		}
	}

	return res;
}

int rio_release_outb_dbell(struct rio_dev *rdev, struct resource *res)
{
	int rc = release_resource(res);

	kfree(res);

	return rc;
}

int rio_request_inb_pwrite(struct rio_dev *rdev,
	int (*pwcback)(struct rio_dev *rdev, union rio_pw_msg *msg, int step))
{
	int rc = 0;

	spin_lock(&rio_global_list_lock);
	if (rdev->pwcback != NULL)
		rc = -ENOMEM;
	else
		rdev->pwcback = pwcback;

	spin_unlock(&rio_global_list_lock);
	return rc;
}
EXPORT_SYMBOL_GPL(rio_request_inb_pwrite);

int rio_release_inb_pwrite(struct rio_dev *rdev)
{
	int rc = -ENOMEM;

	spin_lock(&rio_global_list_lock);
	if (rdev->pwcback) {
		rdev->pwcback = NULL;
		rc = 0;
	}

	spin_unlock(&rio_global_list_lock);
	return rc;
}
EXPORT_SYMBOL_GPL(rio_release_inb_pwrite);

u32
rio_mport_get_physefb(struct rio_mport *port, int local,
		      u16 destid, u8 hopcount)
{
	u32 ext_ftr_ptr;
	u32 ftr_header;

	ext_ftr_ptr = rio_mport_get_efb(port, local, destid, hopcount, 0);

	while (ext_ftr_ptr)  {
		if (local)
			rio_local_read_config_32(port, ext_ftr_ptr,
						 &ftr_header);
		else
			rio_mport_read_config_32(port, destid, hopcount,
						 ext_ftr_ptr, &ftr_header);

		ftr_header = RIO_GET_BLOCK_ID(ftr_header);
		switch (ftr_header) {

		case RIO_EFB_SER_EP_ID_V13P:
		case RIO_EFB_SER_EP_REC_ID_V13P:
		case RIO_EFB_SER_EP_FREE_ID_V13P:
		case RIO_EFB_SER_EP_ID:
		case RIO_EFB_SER_EP_REC_ID:
		case RIO_EFB_SER_EP_FREE_ID:
		case RIO_EFB_SER_EP_FREC_ID:

			return ext_ftr_ptr;

		default:
			break;
		}

		ext_ftr_ptr = rio_mport_get_efb(port, local, destid,
						hopcount, ext_ftr_ptr);
	}

	return ext_ftr_ptr;
}

static struct rio_dev *rio_get_comptag(u32 comp_tag, struct rio_dev *from)
{
	struct list_head *n;
	struct rio_dev *rdev;

	spin_lock(&rio_global_list_lock);
	n = from ? from->global_list.next : rio_devices.next;

	while (n && (n != &rio_devices)) {
		rdev = rio_dev_g(n);
		if (rdev->comp_tag == comp_tag)
			goto exit;
		n = n->next;
	}
	rdev = NULL;
exit:
	spin_unlock(&rio_global_list_lock);
	return rdev;
}

int rio_set_port_lockout(struct rio_dev *rdev, u32 pnum, int lock)
{
	u8 hopcount = 0xff;
	u16 destid = rdev->destid;
	u32 regval;

	if (rdev->rswitch) {
		destid = rdev->rswitch->destid;
		hopcount = rdev->rswitch->hopcount;
	}

	rio_mport_read_config_32(rdev->net->hport, destid, hopcount,
				 rdev->phys_efptr + RIO_PORT_N_CTL_CSR(pnum),
				 &regval);
	if (lock)
		regval |= RIO_PORT_N_CTL_LOCKOUT;
	else
		regval &= ~RIO_PORT_N_CTL_LOCKOUT;

	rio_mport_write_config_32(rdev->net->hport, destid, hopcount,
				  rdev->phys_efptr + RIO_PORT_N_CTL_CSR(pnum),
				  regval);
	return 0;
}

int rio_inb_pwrite_handler(union rio_pw_msg *pw_msg)
{
	struct rio_dev *rdev;
	struct rio_mport *mport;
	u8 hopcount;
	u16 destid;
	u32 err_status;
	int rc, portnum;

	rdev = rio_get_comptag(pw_msg->em.comptag, NULL);
	if (rdev == NULL) {
		/* Someting bad here (probably enumeration error) */
		pr_err("RIO: %s No matching device for CTag 0x%08x\n",
			__func__, pw_msg->em.comptag);
		return -EIO;
	}

	pr_debug("RIO: Port-Write message from %s\n", rio_name(rdev));

#ifdef DEBUG_PW
	{
	u32 i;
	for (i = 0; i < RIO_PW_MSG_SIZE/sizeof(u32);) {
			pr_debug("0x%02x: %08x %08x %08x %08x",
				 i*4, pw_msg->raw[i], pw_msg->raw[i + 1],
				 pw_msg->raw[i + 2], pw_msg->raw[i + 3]);
			i += 4;
	}
	pr_debug("\n");
	}
#endif

	/* Call an external service function (if such is registered
	 * for this device). This may be the service for endpoints that send
	 * device-specific port-write messages. End-point messages expected
	 * to be handled completely by EP specific device driver.
	 * For switches rc==0 signals that no standard processing required.
	 */
	if (rdev->pwcback != NULL) {
		rc = rdev->pwcback(rdev, pw_msg, 0);
		if (rc == 0)
			return 0;
	}

	/* For End-point devices processing stops here */
	if (!(rdev->pef & RIO_PEF_SWITCH))
		return 0;

	if (rdev->phys_efptr == 0) {
		pr_err("RIO_PW: Bad switch initialization for %s\n",
			rio_name(rdev));
		return 0;
	}

	mport = rdev->net->hport;
	destid = rdev->rswitch->destid;
	hopcount = rdev->rswitch->hopcount;

	/*
	 * Process the port-write notification from switch
	 */

	portnum = pw_msg->em.is_port & 0xFF;

	if (rdev->rswitch->em_handle)
		rdev->rswitch->em_handle(rdev, portnum);

	rio_mport_read_config_32(mport, destid, hopcount,
			rdev->phys_efptr + RIO_PORT_N_ERR_STS_CSR(portnum),
			&err_status);
	pr_debug("RIO_PW: SP%d_ERR_STS_CSR=0x%08x\n", portnum, err_status);

	if (pw_msg->em.errdetect) {
		pr_debug("RIO_PW: RIO_EM_P%d_ERR_DETECT=0x%08x\n",
			 portnum, pw_msg->em.errdetect);
		/* Clear EM Port N Error Detect CSR */
		rio_mport_write_config_32(mport, destid, hopcount,
			rdev->em_efptr + RIO_EM_PN_ERR_DETECT(portnum), 0);
	}

	if (pw_msg->em.ltlerrdet) {
		pr_debug("RIO_PW: RIO_EM_LTL_ERR_DETECT=0x%08x\n",
			 pw_msg->em.ltlerrdet);
		/* Clear EM L/T Layer Error Detect CSR */
		rio_mport_write_config_32(mport, destid, hopcount,
			rdev->em_efptr + RIO_EM_LTL_ERR_DETECT, 0);
	}

	/* Clear Port Errors */
	rio_mport_write_config_32(mport, destid, hopcount,
			rdev->phys_efptr + RIO_PORT_N_ERR_STS_CSR(portnum),
			err_status & RIO_PORT_N_ERR_STS_CLR_MASK);

	if (rdev->rswitch->port_ok & (1 << portnum)) {
		if (err_status & RIO_PORT_N_ERR_STS_PORT_UNINIT) {
			rdev->rswitch->port_ok &= ~(1 << portnum);
			rio_set_port_lockout(rdev, portnum, 1);

			rio_mport_write_config_32(mport, destid, hopcount,
				rdev->phys_efptr +
					RIO_PORT_N_ACK_STS_CSR(portnum),
				RIO_PORT_N_ACK_CLEAR);

			/* Schedule Extraction Service */
			pr_debug("RIO_PW: Device Extraction on [%s]-P%d\n",
			       rio_name(rdev), portnum);
		}
	} else {
		if (err_status & RIO_PORT_N_ERR_STS_PORT_OK) {
			rdev->rswitch->port_ok |= (1 << portnum);
			rio_set_port_lockout(rdev, portnum, 0);

			/* Schedule Insertion Service */
			pr_debug("RIO_PW: Device Insertion on [%s]-P%d\n",
			       rio_name(rdev), portnum);
		}
	}

	/* Clear Port-Write Pending bit */
	rio_mport_write_config_32(mport, destid, hopcount,
			rdev->phys_efptr + RIO_PORT_N_ERR_STS_CSR(portnum),
			RIO_PORT_N_ERR_STS_PW_PEND);

	return 0;
}
EXPORT_SYMBOL_GPL(rio_inb_pwrite_handler);

u32
rio_mport_get_efb(struct rio_mport *port, int local, u16 destid,
		      u8 hopcount, u32 from)
{
	u32 reg_val;

	if (from == 0) {
		if (local)
			rio_local_read_config_32(port, RIO_ASM_INFO_CAR,
						 &reg_val);
		else
			rio_mport_read_config_32(port, destid, hopcount,
						 RIO_ASM_INFO_CAR, &reg_val);
		return reg_val & RIO_EXT_FTR_PTR_MASK;
	} else {
		if (local)
			rio_local_read_config_32(port, from, &reg_val);
		else
			rio_mport_read_config_32(port, destid, hopcount,
						 from, &reg_val);
		return RIO_GET_BLOCK_ID(reg_val);
	}
}

u32
rio_mport_get_feature(struct rio_mport * port, int local, u16 destid,
		      u8 hopcount, int ftr)
{
	u32 asm_info, ext_ftr_ptr, ftr_header;

	if (local)
		rio_local_read_config_32(port, RIO_ASM_INFO_CAR, &asm_info);
	else
		rio_mport_read_config_32(port, destid, hopcount,
					 RIO_ASM_INFO_CAR, &asm_info);

	ext_ftr_ptr = asm_info & RIO_EXT_FTR_PTR_MASK;

	while (ext_ftr_ptr) {
		if (local)
			rio_local_read_config_32(port, ext_ftr_ptr,
						 &ftr_header);
		else
			rio_mport_read_config_32(port, destid, hopcount,
						 ext_ftr_ptr, &ftr_header);
		if (RIO_GET_BLOCK_ID(ftr_header) == ftr)
			return ext_ftr_ptr;
		if (!(ext_ftr_ptr = RIO_GET_BLOCK_PTR(ftr_header)))
			break;
	}

	return 0;
}

struct rio_dev *rio_get_asm(u16 vid, u16 did,
			    u16 asm_vid, u16 asm_did, struct rio_dev *from)
{
	struct list_head *n;
	struct rio_dev *rdev;

	WARN_ON(in_interrupt());
	spin_lock(&rio_global_list_lock);
	n = from ? from->global_list.next : rio_devices.next;

	while (n && (n != &rio_devices)) {
		rdev = rio_dev_g(n);
		if ((vid == RIO_ANY_ID || rdev->vid == vid) &&
		    (did == RIO_ANY_ID || rdev->did == did) &&
		    (asm_vid == RIO_ANY_ID || rdev->asm_vid == asm_vid) &&
		    (asm_did == RIO_ANY_ID || rdev->asm_did == asm_did))
			goto exit;
		n = n->next;
	}
	rdev = NULL;
      exit:
	rio_dev_put(from);
	rdev = rio_dev_get(rdev);
	spin_unlock(&rio_global_list_lock);
	return rdev;
}

struct rio_dev *rio_get_device(u16 vid, u16 did, struct rio_dev *from)
{
	return rio_get_asm(vid, did, RIO_ANY_ID, RIO_ANY_ID, from);
}

int rio_std_route_add_entry(struct rio_mport *mport, u16 destid, u8 hopcount,
		       u16 table, u16 route_destid, u8 route_port)
{
	if (table == RIO_GLOBAL_TABLE) {
		rio_mport_write_config_32(mport, destid, hopcount,
				RIO_STD_RTE_CONF_DESTID_SEL_CSR,
				(u32)route_destid);
		rio_mport_write_config_32(mport, destid, hopcount,
				RIO_STD_RTE_CONF_PORT_SEL_CSR,
				(u32)route_port);
	}

	udelay(10);
	return 0;
}

int rio_std_route_get_entry(struct rio_mport *mport, u16 destid, u8 hopcount,
		       u16 table, u16 route_destid, u8 *route_port)
{
	u32 result;

	if (table == RIO_GLOBAL_TABLE) {
		rio_mport_write_config_32(mport, destid, hopcount,
				RIO_STD_RTE_CONF_DESTID_SEL_CSR, route_destid);
		rio_mport_read_config_32(mport, destid, hopcount,
				RIO_STD_RTE_CONF_PORT_SEL_CSR, &result);

		*route_port = (u8)result;
	}

	return 0;
}

int rio_std_route_clr_table(struct rio_mport *mport, u16 destid, u8 hopcount,
		       u16 table)
{
	u32 max_destid = 0xff;
	u32 i, pef, id_inc = 1, ext_cfg = 0;
	u32 port_sel = RIO_INVALID_ROUTE;

	if (table == RIO_GLOBAL_TABLE) {
		rio_mport_read_config_32(mport, destid, hopcount,
					 RIO_PEF_CAR, &pef);

		if (mport->sys_size) {
			rio_mport_read_config_32(mport, destid, hopcount,
						 RIO_SWITCH_RT_LIMIT,
						 &max_destid);
			max_destid &= RIO_RT_MAX_DESTID;
		}

		if (pef & RIO_PEF_EXT_RT) {
			ext_cfg = 0x80000000;
			id_inc = 4;
			port_sel = (RIO_INVALID_ROUTE << 24) |
				   (RIO_INVALID_ROUTE << 16) |
				   (RIO_INVALID_ROUTE << 8) |
				   RIO_INVALID_ROUTE;
		}

		for (i = 0; i <= max_destid;) {
			rio_mport_write_config_32(mport, destid, hopcount,
					RIO_STD_RTE_CONF_DESTID_SEL_CSR,
					ext_cfg | i);
			rio_mport_write_config_32(mport, destid, hopcount,
					RIO_STD_RTE_CONF_PORT_SEL_CSR,
					port_sel);
			i += id_inc;
		}
	}

	udelay(10);
	return 0;
}

static void rio_fixup_device(struct rio_dev *dev)
{
}

static int __devinit rio_init(void)
{
	struct rio_dev *dev = NULL;

	while ((dev = rio_get_device(RIO_ANY_ID, RIO_ANY_ID, dev)) != NULL) {
		rio_fixup_device(dev);
	}
	return 0;
}

device_initcall(rio_init);

int __devinit rio_init_mports(void)
{
	int rc = 0;
	struct rio_mport *port;

	list_for_each_entry(port, &rio_mports, node) {
		if (!request_mem_region(port->iores.start,
					port->iores.end - port->iores.start,
					port->name)) {
			printk(KERN_ERR
			       "RIO: Error requesting master port region 0x%016llx-0x%016llx\n",
			       (u64)port->iores.start, (u64)port->iores.end - 1);
			rc = -ENOMEM;
			goto out;
		}

		if (port->host_deviceid >= 0)
			rio_enum_mport(port);
		else
			rio_disc_mport(port);
	}

      out:
	return rc;
}

void rio_register_mport(struct rio_mport *port)
{
	list_add_tail(&port->node, &rio_mports);
}

EXPORT_SYMBOL_GPL(rio_local_get_device_id);
EXPORT_SYMBOL_GPL(rio_get_device);
EXPORT_SYMBOL_GPL(rio_get_asm);
EXPORT_SYMBOL_GPL(rio_request_inb_dbell);
EXPORT_SYMBOL_GPL(rio_release_inb_dbell);
EXPORT_SYMBOL_GPL(rio_request_outb_dbell);
EXPORT_SYMBOL_GPL(rio_release_outb_dbell);
EXPORT_SYMBOL_GPL(rio_request_inb_mbox);
EXPORT_SYMBOL_GPL(rio_release_inb_mbox);
EXPORT_SYMBOL_GPL(rio_request_outb_mbox);
EXPORT_SYMBOL_GPL(rio_release_outb_mbox);
