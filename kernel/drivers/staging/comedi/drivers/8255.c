


#include "../comedidev.h"

#include <linux/ioport.h>
#include <linux/slab.h>
#include "8255.h"

#define _8255_SIZE 4

#define _8255_DATA 0
#define _8255_CR 3

#define CR_C_LO_IO	0x01
#define CR_B_IO		0x02
#define CR_B_MODE	0x04
#define CR_C_HI_IO	0x08
#define CR_A_IO		0x10
#define CR_A_MODE(a)	((a)<<5)
#define CR_CW		0x80

struct subdev_8255_struct {
	unsigned long cb_arg;
	int (*cb_func) (int, int, int, unsigned long);
	int have_irq;
};

#define CALLBACK_ARG	(((struct subdev_8255_struct *)s->private)->cb_arg)
#define CALLBACK_FUNC	(((struct subdev_8255_struct *)s->private)->cb_func)
#define subdevpriv	((struct subdev_8255_struct *)s->private)

static int dev_8255_attach(struct comedi_device *dev,
			   struct comedi_devconfig *it);
static int dev_8255_detach(struct comedi_device *dev);
static struct comedi_driver driver_8255 = {
	.driver_name = "8255",
	.module = THIS_MODULE,
	.attach = dev_8255_attach,
	.detach = dev_8255_detach,
};

COMEDI_INITCLEANUP(driver_8255);

static void do_config(struct comedi_device *dev, struct comedi_subdevice *s);

void subdev_8255_interrupt(struct comedi_device *dev,
			   struct comedi_subdevice *s)
{
	short d;

	d = CALLBACK_FUNC(0, _8255_DATA, 0, CALLBACK_ARG);
	d |= (CALLBACK_FUNC(0, _8255_DATA + 1, 0, CALLBACK_ARG) << 8);

	comedi_buf_put(s->async, d);
	s->async->events |= COMEDI_CB_EOS;

	comedi_event(dev, s);
}
EXPORT_SYMBOL(subdev_8255_interrupt);

static int subdev_8255_cb(int dir, int port, int data, unsigned long arg)
{
	unsigned long iobase = arg;

	if (dir) {
		outb(data, iobase + port);
		return 0;
	} else {
		return inb(iobase + port);
	}
}

static int subdev_8255_insn(struct comedi_device *dev,
			    struct comedi_subdevice *s,
			    struct comedi_insn *insn, unsigned int *data)
{
	if (data[0]) {
		s->state &= ~data[0];
		s->state |= (data[0] & data[1]);

		if (data[0] & 0xff)
			CALLBACK_FUNC(1, _8255_DATA, s->state & 0xff,
				      CALLBACK_ARG);
		if (data[0] & 0xff00)
			CALLBACK_FUNC(1, _8255_DATA + 1, (s->state >> 8) & 0xff,
				      CALLBACK_ARG);
		if (data[0] & 0xff0000)
			CALLBACK_FUNC(1, _8255_DATA + 2,
				      (s->state >> 16) & 0xff, CALLBACK_ARG);
	}

	data[1] = CALLBACK_FUNC(0, _8255_DATA, 0, CALLBACK_ARG);
	data[1] |= (CALLBACK_FUNC(0, _8255_DATA + 1, 0, CALLBACK_ARG) << 8);
	data[1] |= (CALLBACK_FUNC(0, _8255_DATA + 2, 0, CALLBACK_ARG) << 16);

	return 2;
}

static int subdev_8255_insn_config(struct comedi_device *dev,
				   struct comedi_subdevice *s,
				   struct comedi_insn *insn, unsigned int *data)
{
	unsigned int mask;
	unsigned int bits;

	mask = 1 << CR_CHAN(insn->chanspec);
	if (mask & 0x0000ff)
		bits = 0x0000ff;
	else if (mask & 0x00ff00)
		bits = 0x00ff00;
	else if (mask & 0x0f0000)
		bits = 0x0f0000;
	else
		bits = 0xf00000;

	switch (data[0]) {
	case INSN_CONFIG_DIO_INPUT:
		s->io_bits &= ~bits;
		break;
	case INSN_CONFIG_DIO_OUTPUT:
		s->io_bits |= bits;
		break;
	case INSN_CONFIG_DIO_QUERY:
		data[1] = (s->io_bits & bits) ? COMEDI_OUTPUT : COMEDI_INPUT;
		return insn->n;
		break;
	default:
		return -EINVAL;
	}

	do_config(dev, s);

	return 1;
}

static void do_config(struct comedi_device *dev, struct comedi_subdevice *s)
{
	int config;

	config = CR_CW;
	/* 1 in io_bits indicates output, 1 in config indicates input */
	if (!(s->io_bits & 0x0000ff))
		config |= CR_A_IO;
	if (!(s->io_bits & 0x00ff00))
		config |= CR_B_IO;
	if (!(s->io_bits & 0x0f0000))
		config |= CR_C_LO_IO;
	if (!(s->io_bits & 0xf00000))
		config |= CR_C_HI_IO;
	CALLBACK_FUNC(1, _8255_CR, config, CALLBACK_ARG);
}

static int subdev_8255_cmdtest(struct comedi_device *dev,
			       struct comedi_subdevice *s,
			       struct comedi_cmd *cmd)
{
	int err = 0;
	unsigned int tmp;

	/* step 1 */

	tmp = cmd->start_src;
	cmd->start_src &= TRIG_NOW;
	if (!cmd->start_src || tmp != cmd->start_src)
		err++;

	tmp = cmd->scan_begin_src;
	cmd->scan_begin_src &= TRIG_EXT;
	if (!cmd->scan_begin_src || tmp != cmd->scan_begin_src)
		err++;

	tmp = cmd->convert_src;
	cmd->convert_src &= TRIG_FOLLOW;
	if (!cmd->convert_src || tmp != cmd->convert_src)
		err++;

	tmp = cmd->scan_end_src;
	cmd->scan_end_src &= TRIG_COUNT;
	if (!cmd->scan_end_src || tmp != cmd->scan_end_src)
		err++;

	tmp = cmd->stop_src;
	cmd->stop_src &= TRIG_NONE;
	if (!cmd->stop_src || tmp != cmd->stop_src)
		err++;

	if (err)
		return 1;

	/* step 2 */

	if (err)
		return 2;

	/* step 3 */

	if (cmd->start_arg != 0) {
		cmd->start_arg = 0;
		err++;
	}
	if (cmd->scan_begin_arg != 0) {
		cmd->scan_begin_arg = 0;
		err++;
	}
	if (cmd->convert_arg != 0) {
		cmd->convert_arg = 0;
		err++;
	}
	if (cmd->scan_end_arg != 1) {
		cmd->scan_end_arg = 1;
		err++;
	}
	if (cmd->stop_arg != 0) {
		cmd->stop_arg = 0;
		err++;
	}

	if (err)
		return 3;

	/* step 4 */

	if (err)
		return 4;

	return 0;
}

static int subdev_8255_cmd(struct comedi_device *dev,
			   struct comedi_subdevice *s)
{
	/* FIXME */

	return 0;
}

static int subdev_8255_cancel(struct comedi_device *dev,
			      struct comedi_subdevice *s)
{
	/* FIXME */

	return 0;
}

int subdev_8255_init(struct comedi_device *dev, struct comedi_subdevice *s,
		     int (*cb) (int, int, int, unsigned long),
		     unsigned long arg)
{
	s->type = COMEDI_SUBD_DIO;
	s->subdev_flags = SDF_READABLE | SDF_WRITABLE;
	s->n_chan = 24;
	s->range_table = &range_digital;
	s->maxdata = 1;

	s->private = kmalloc(sizeof(struct subdev_8255_struct), GFP_KERNEL);
	if (!s->private)
		return -ENOMEM;

	CALLBACK_ARG = arg;
	if (cb == NULL)
		CALLBACK_FUNC = subdev_8255_cb;
	else
		CALLBACK_FUNC = cb;
	s->insn_bits = subdev_8255_insn;
	s->insn_config = subdev_8255_insn_config;

	s->state = 0;
	s->io_bits = 0;
	do_config(dev, s);

	return 0;
}
EXPORT_SYMBOL(subdev_8255_init);

int subdev_8255_init_irq(struct comedi_device *dev, struct comedi_subdevice *s,
			 int (*cb) (int, int, int, unsigned long),
			 unsigned long arg)
{
	int ret;

	ret = subdev_8255_init(dev, s, cb, arg);
	if (ret < 0)
		return ret;

	s->do_cmdtest = subdev_8255_cmdtest;
	s->do_cmd = subdev_8255_cmd;
	s->cancel = subdev_8255_cancel;

	subdevpriv->have_irq = 1;

	return 0;
}
EXPORT_SYMBOL(subdev_8255_init_irq);

void subdev_8255_cleanup(struct comedi_device *dev, struct comedi_subdevice *s)
{
	if (s->private) {
		/* this test does nothing, so comment it out
		 * if (subdevpriv->have_irq) {
		 * }
		 */

		kfree(s->private);
	}
}
EXPORT_SYMBOL(subdev_8255_cleanup);


static int dev_8255_attach(struct comedi_device *dev,
			   struct comedi_devconfig *it)
{
	int ret;
	unsigned long iobase;
	int i;

	dev->board_name = "8255";

	for (i = 0; i < COMEDI_NDEVCONFOPTS; i++) {
		iobase = it->options[i];
		if (!iobase)
			break;
	}
	if (i == 0) {
		printk(KERN_WARNING
		       "comedi%d: 8255: no devices specified\n", dev->minor);
		return -EINVAL;
	}

	ret = alloc_subdevices(dev, i);
	if (ret < 0) {
		/* FIXME this printk call should give a proper message, the
		 * below line just maintains previous functionality */
		printk("comedi%d: 8255:", dev->minor);
		return ret;
	}

	printk(KERN_INFO "comedi%d: 8255:", dev->minor);

	for (i = 0; i < dev->n_subdevices; i++) {
		iobase = it->options[i];

		printk(" 0x%04lx", iobase);
		if (!request_region(iobase, _8255_SIZE, "8255")) {
			printk(" (I/O port conflict)");

			dev->subdevices[i].type = COMEDI_SUBD_UNUSED;
		} else {
			subdev_8255_init(dev, dev->subdevices + i, NULL,
					 iobase);
		}
	}

	printk("\n");

	return 0;
}

static int dev_8255_detach(struct comedi_device *dev)
{
	int i;
	unsigned long iobase;
	struct comedi_subdevice *s;

	printk(KERN_INFO "comedi%d: 8255: remove\n", dev->minor);

	for (i = 0; i < dev->n_subdevices; i++) {
		s = dev->subdevices + i;
		if (s->type != COMEDI_SUBD_UNUSED) {
			iobase = CALLBACK_ARG;
			release_region(iobase, _8255_SIZE);
		}
		subdev_8255_cleanup(dev, s);
	}

	return 0;
}
