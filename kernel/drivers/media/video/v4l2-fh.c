

#include <linux/bitops.h>
#include <media/v4l2-dev.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-event.h>
#include <media/v4l2-ioctl.h>

int v4l2_fh_init(struct v4l2_fh *fh, struct video_device *vdev)
{
	fh->vdev = vdev;
	INIT_LIST_HEAD(&fh->list);
	set_bit(V4L2_FL_USES_V4L2_FH, &fh->vdev->flags);

	/*
	 * fh->events only needs to be initialized if the driver
	 * supports the VIDIOC_SUBSCRIBE_EVENT ioctl.
	 */
	if (vdev->ioctl_ops && vdev->ioctl_ops->vidioc_subscribe_event)
		return v4l2_event_init(fh);

	fh->events = NULL;

	return 0;
}
EXPORT_SYMBOL_GPL(v4l2_fh_init);

void v4l2_fh_add(struct v4l2_fh *fh)
{
	unsigned long flags;

	spin_lock_irqsave(&fh->vdev->fh_lock, flags);
	list_add(&fh->list, &fh->vdev->fh_list);
	spin_unlock_irqrestore(&fh->vdev->fh_lock, flags);
}
EXPORT_SYMBOL_GPL(v4l2_fh_add);

void v4l2_fh_del(struct v4l2_fh *fh)
{
	unsigned long flags;

	spin_lock_irqsave(&fh->vdev->fh_lock, flags);
	list_del_init(&fh->list);
	spin_unlock_irqrestore(&fh->vdev->fh_lock, flags);
}
EXPORT_SYMBOL_GPL(v4l2_fh_del);

void v4l2_fh_exit(struct v4l2_fh *fh)
{
	if (fh->vdev == NULL)
		return;

	fh->vdev = NULL;

	v4l2_event_free(fh);
}
EXPORT_SYMBOL_GPL(v4l2_fh_exit);
