

#define ZR016_VERSION "v0.7"

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/delay.h>

#include <linux/types.h>
#include <linux/wait.h>

/* I/O commands, error codes */
#include <asm/io.h>

/* v4l  API */

/* headerfile of this module */
#include"zr36016.h"

/* codec io API */
#include"videocodec.h"

#define MAX_CODECS 20

/* amount of chips attached via this driver */
static int zr36016_codecs;

/* debugging is available via module parameter */
static int debug;
module_param(debug, int, 0);
MODULE_PARM_DESC(debug, "Debug level (0-4)");

#define dprintk(num, format, args...) \
	do { \
		if (debug >= num) \
			printk(format, ##args); \
	} while (0)


/* read and write functions */
static u8
zr36016_read (struct zr36016 *ptr,
	      u16             reg)
{
	u8 value = 0;

	// just in case something is wrong...
	if (ptr->codec->master_data->readreg)
		value =
		    (ptr->codec->master_data->
		     readreg(ptr->codec, reg)) & 0xFF;
	else
		dprintk(1,
			KERN_ERR "%s: invalid I/O setup, nothing read!\n",
			ptr->name);

	dprintk(4, "%s: reading from 0x%04x: %02x\n", ptr->name, reg,
		value);

	return value;
}

static void
zr36016_write (struct zr36016 *ptr,
	       u16             reg,
	       u8              value)
{
	dprintk(4, "%s: writing 0x%02x to 0x%04x\n", ptr->name, value,
		reg);

	// just in case something is wrong...
	if (ptr->codec->master_data->writereg) {
		ptr->codec->master_data->writereg(ptr->codec, reg, value);
	} else
		dprintk(1,
			KERN_ERR
			"%s: invalid I/O setup, nothing written!\n",
			ptr->name);
}

/* indirect read and write functions */
static u8
zr36016_readi (struct zr36016 *ptr,
	       u16             reg)
{
	u8 value = 0;

	// just in case something is wrong...
	if ((ptr->codec->master_data->writereg) &&
	    (ptr->codec->master_data->readreg)) {
		ptr->codec->master_data->writereg(ptr->codec, ZR016_IADDR, reg & 0x0F);	// ADDR
		value = (ptr->codec->master_data->readreg(ptr->codec, ZR016_IDATA)) & 0xFF;	// DATA
	} else
		dprintk(1,
			KERN_ERR
			"%s: invalid I/O setup, nothing read (i)!\n",
			ptr->name);

	dprintk(4, "%s: reading indirect from 0x%04x: %02x\n", ptr->name,
		reg, value);
	return value;
}

static void
zr36016_writei (struct zr36016 *ptr,
		u16             reg,
		u8              value)
{
	dprintk(4, "%s: writing indirect 0x%02x to 0x%04x\n", ptr->name,
		value, reg);

	// just in case something is wrong...
	if (ptr->codec->master_data->writereg) {
		ptr->codec->master_data->writereg(ptr->codec, ZR016_IADDR, reg & 0x0F);	// ADDR
		ptr->codec->master_data->writereg(ptr->codec, ZR016_IDATA, value & 0x0FF);	// DATA
	} else
		dprintk(1,
			KERN_ERR
			"%s: invalid I/O setup, nothing written (i)!\n",
			ptr->name);
}


/* version kept in datastructure */
static u8
zr36016_read_version (struct zr36016 *ptr)
{
	ptr->version = zr36016_read(ptr, 0) >> 4;
	return ptr->version;
}


static int
zr36016_basic_test (struct zr36016 *ptr)
{
	if (debug) {
		int i;
		zr36016_writei(ptr, ZR016I_PAX_LO, 0x55);
		dprintk(1, KERN_INFO "%s: registers: ", ptr->name);
		for (i = 0; i <= 0x0b; i++)
			dprintk(1, "%02x ", zr36016_readi(ptr, i));
		dprintk(1, "\n");
	}
	// for testing just write 0, then the default value to a register and read
	// it back in both cases
	zr36016_writei(ptr, ZR016I_PAX_LO, 0x00);
	if (zr36016_readi(ptr, ZR016I_PAX_LO) != 0x0) {
		dprintk(1,
			KERN_ERR
			"%s: attach failed, can't connect to vfe processor!\n",
			ptr->name);
		return -ENXIO;
	}
	zr36016_writei(ptr, ZR016I_PAX_LO, 0x0d0);
	if (zr36016_readi(ptr, ZR016I_PAX_LO) != 0x0d0) {
		dprintk(1,
			KERN_ERR
			"%s: attach failed, can't connect to vfe processor!\n",
			ptr->name);
		return -ENXIO;
	}
	// we allow version numbers from 0-3, should be enough, though
	zr36016_read_version(ptr);
	if (ptr->version & 0x0c) {
		dprintk(1,
			KERN_ERR
			"%s: attach failed, suspicious version %d found...\n",
			ptr->name, ptr->version);
		return -ENXIO;
	}

	return 0;		/* looks good! */
}


#if 0
static int zr36016_pushit (struct zr36016 *ptr,
			   u16             startreg,
			   u16             len,
			   const char     *data)
{
	int i=0;

	dprintk(4, "%s: write data block to 0x%04x (len=%d)\n",
		ptr->name, startreg,len);
	while (i<len) {
		zr36016_writei(ptr, startreg++,  data[i++]);
	}

	return i;
}
#endif


// needed offset values          PAL NTSC SECAM
static const int zr016_xoff[] = { 20, 20, 20 };
static const int zr016_yoff[] = { 8, 9, 7 };

static void
zr36016_init (struct zr36016 *ptr)
{
	// stop any processing
	zr36016_write(ptr, ZR016_GOSTOP, 0);

	// mode setup (yuv422 in and out, compression/expansuon due to mode)
	zr36016_write(ptr, ZR016_MODE,
		      ZR016_YUV422 | ZR016_YUV422_YUV422 |
		      (ptr->mode == CODEC_DO_COMPRESSION ?
		       ZR016_COMPRESSION : ZR016_EXPANSION));

	// misc setup
	zr36016_writei(ptr, ZR016I_SETUP1,
		       (ptr->xdec ? (ZR016_HRFL | ZR016_HORZ) : 0) |
		       (ptr->ydec ? ZR016_VERT : 0) | ZR016_CNTI);
	zr36016_writei(ptr, ZR016I_SETUP2, ZR016_CCIR);

	// Window setup
	// (no extra offset for now, norm defines offset, default width height)
	zr36016_writei(ptr, ZR016I_PAX_HI, ptr->width >> 8);
	zr36016_writei(ptr, ZR016I_PAX_LO, ptr->width & 0xFF);
	zr36016_writei(ptr, ZR016I_PAY_HI, ptr->height >> 8);
	zr36016_writei(ptr, ZR016I_PAY_LO, ptr->height & 0xFF);
	zr36016_writei(ptr, ZR016I_NAX_HI, ptr->xoff >> 8);
	zr36016_writei(ptr, ZR016I_NAX_LO, ptr->xoff & 0xFF);
	zr36016_writei(ptr, ZR016I_NAY_HI, ptr->yoff >> 8);
	zr36016_writei(ptr, ZR016I_NAY_LO, ptr->yoff & 0xFF);

	/* shall we continue now, please? */
	zr36016_write(ptr, ZR016_GOSTOP, 1);
}


static int
zr36016_set_mode (struct videocodec *codec,
		  int                mode)
{
	struct zr36016 *ptr = (struct zr36016 *) codec->data;

	dprintk(2, "%s: set_mode %d call\n", ptr->name, mode);

	if ((mode != CODEC_DO_EXPANSION) && (mode != CODEC_DO_COMPRESSION))
		return -EINVAL;

	ptr->mode = mode;
	zr36016_init(ptr);

	return 0;
}

/* set picture size */
static int
zr36016_set_video (struct videocodec   *codec,
		   struct tvnorm       *norm,
		   struct vfe_settings *cap,
		   struct vfe_polarity *pol)
{
	struct zr36016 *ptr = (struct zr36016 *) codec->data;

	dprintk(2, "%s: set_video %d.%d, %d/%d-%dx%d (0x%x) call\n",
		ptr->name, norm->HStart, norm->VStart,
		cap->x, cap->y, cap->width, cap->height,
		cap->decimation);

	/* if () return -EINVAL;
	 * trust the master driver that it knows what it does - so
	 * we allow invalid startx/y for now ... */
	ptr->width = cap->width;
	ptr->height = cap->height;
	/* (Ronald) This is ugly. zoran_device.c, line 387
	 * already mentions what happens if HStart is even
	 * (blue faces, etc., cr/cb inversed). There's probably
	 * some good reason why HStart is 0 instead of 1, so I'm
	 * leaving it to this for now, but really... This can be
	 * done a lot simpler */
	ptr->xoff = (norm->HStart ? norm->HStart : 1) + cap->x;
	/* Something to note here (I don't understand it), setting
	 * VStart too high will cause the codec to 'not work'. I
	 * really don't get it. values of 16 (VStart) already break
	 * it here. Just '0' seems to work. More testing needed! */
	ptr->yoff = norm->VStart + cap->y;
	/* (Ronald) dzjeeh, can't this thing do hor_decimation = 4? */
	ptr->xdec = ((cap->decimation & 0xff) == 1) ? 0 : 1;
	ptr->ydec = (((cap->decimation >> 8) & 0xff) == 1) ? 0 : 1;

	return 0;
}

/* additional control functions */
static int
zr36016_control (struct videocodec *codec,
		 int                type,
		 int                size,
		 void              *data)
{
	struct zr36016 *ptr = (struct zr36016 *) codec->data;
	int *ival = (int *) data;

	dprintk(2, "%s: control %d call with %d byte\n", ptr->name, type,
		size);

	switch (type) {
	case CODEC_G_STATUS:	/* get last status - we don't know it ... */
		if (size != sizeof(int))
			return -EFAULT;
		*ival = 0;
		break;

	case CODEC_G_CODEC_MODE:
		if (size != sizeof(int))
			return -EFAULT;
		*ival = 0;
		break;

	case CODEC_S_CODEC_MODE:
		if (size != sizeof(int))
			return -EFAULT;
		if (*ival != 0)
			return -EINVAL;
		/* not needed, do nothing */
		return 0;

	case CODEC_G_VFE:
	case CODEC_S_VFE:
		return 0;

	case CODEC_S_MMAP:
		/* not available, give an error */
		return -ENXIO;

	default:
		return -EINVAL;
	}

	return size;
}


static int
zr36016_unset (struct videocodec *codec)
{
	struct zr36016 *ptr = codec->data;

	if (ptr) {
		/* do wee need some codec deinit here, too ???? */

		dprintk(1, "%s: finished codec #%d\n", ptr->name,
			ptr->num);
		kfree(ptr);
		codec->data = NULL;

		zr36016_codecs--;
		return 0;
	}

	return -EFAULT;
}


static int
zr36016_setup (struct videocodec *codec)
{
	struct zr36016 *ptr;
	int res;

	dprintk(2, "zr36016: initializing VFE subsystem #%d.\n",
		zr36016_codecs);

	if (zr36016_codecs == MAX_CODECS) {
		dprintk(1,
			KERN_ERR "zr36016: Can't attach more codecs!\n");
		return -ENOSPC;
	}
	//mem structure init
	codec->data = ptr = kzalloc(sizeof(struct zr36016), GFP_KERNEL);
	if (NULL == ptr) {
		dprintk(1, KERN_ERR "zr36016: Can't get enough memory!\n");
		return -ENOMEM;
	}

	snprintf(ptr->name, sizeof(ptr->name), "zr36016[%d]",
		 zr36016_codecs);
	ptr->num = zr36016_codecs++;
	ptr->codec = codec;

	//testing
	res = zr36016_basic_test(ptr);
	if (res < 0) {
		zr36016_unset(codec);
		return res;
	}
	//final setup
	ptr->mode = CODEC_DO_COMPRESSION;
	ptr->width = 768;
	ptr->height = 288;
	ptr->xdec = 1;
	ptr->ydec = 0;
	zr36016_init(ptr);

	dprintk(1, KERN_INFO "%s: codec v%d attached and running\n",
		ptr->name, ptr->version);

	return 0;
}

static const struct videocodec zr36016_codec = {
	.owner = THIS_MODULE,
	.name = "zr36016",
	.magic = 0L,		// magic not used
	.flags =
	    CODEC_FLAG_HARDWARE | CODEC_FLAG_VFE | CODEC_FLAG_ENCODER |
	    CODEC_FLAG_DECODER,
	.type = CODEC_TYPE_ZR36016,
	.setup = zr36016_setup,	// functionality
	.unset = zr36016_unset,
	.set_mode = zr36016_set_mode,
	.set_video = zr36016_set_video,
	.control = zr36016_control,
	// others are not used
};


static int __init
zr36016_init_module (void)
{
	//dprintk(1, "ZR36016 driver %s\n",ZR016_VERSION);
	zr36016_codecs = 0;
	return videocodec_register(&zr36016_codec);
}

static void __exit
zr36016_cleanup_module (void)
{
	if (zr36016_codecs) {
		dprintk(1,
			"zr36016: something's wrong - %d codecs left somehow.\n",
			zr36016_codecs);
	}
	videocodec_unregister(&zr36016_codec);
}

module_init(zr36016_init_module);
module_exit(zr36016_cleanup_module);

MODULE_AUTHOR("Wolfgang Scherr <scherr@net4you.at>");
MODULE_DESCRIPTION("Driver module for ZR36016 video frontends "
		   ZR016_VERSION);
MODULE_LICENSE("GPL");
