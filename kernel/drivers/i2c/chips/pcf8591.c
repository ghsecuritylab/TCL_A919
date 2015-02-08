

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/mutex.h>

/* Addresses to scan */
static const unsigned short normal_i2c[] = { 0x48, 0x49, 0x4a, 0x4b, 0x4c,
					0x4d, 0x4e, 0x4f, I2C_CLIENT_END };

/* Insmod parameters */
I2C_CLIENT_INSMOD_1(pcf8591);

static int input_mode;
module_param(input_mode, int, 0);
MODULE_PARM_DESC(input_mode,
	"Analog input mode:\n"
	" 0 = four single ended inputs\n"
	" 1 = three differential inputs\n"
	" 2 = single ended and differential mixed\n"
	" 3 = two differential inputs\n");


/* Analog Output Enable Flag (analog output active if 1) */
#define PCF8591_CONTROL_AOEF		0x40
					
#define PCF8591_CONTROL_AIP_MASK	0x30

/* Autoincrement Flag (switch on if 1) */
#define PCF8591_CONTROL_AINC		0x04

#define PCF8591_CONTROL_AICH_MASK	0x03

/* Initial values */
#define PCF8591_INIT_CONTROL	((input_mode << 4) | PCF8591_CONTROL_AOEF)
#define PCF8591_INIT_AOUT	0	/* DAC out = 0 */

/* Conversions */
#define REG_TO_SIGNED(reg)	(((reg) & 0x80)?((reg) - 256):(reg))

struct pcf8591_data {
	struct mutex update_lock;

	u8 control;
	u8 aout;
};

static void pcf8591_init_client(struct i2c_client *client);
static int pcf8591_read_channel(struct device *dev, int channel);

/* following are the sysfs callback functions */
#define show_in_channel(channel)					\
static ssize_t show_in##channel##_input(struct device *dev, struct device_attribute *attr, char *buf)	\
{									\
	return sprintf(buf, "%d\n", pcf8591_read_channel(dev, channel));\
}									\
static DEVICE_ATTR(in##channel##_input, S_IRUGO,			\
		   show_in##channel##_input, NULL);

show_in_channel(0);
show_in_channel(1);
show_in_channel(2);
show_in_channel(3);

static ssize_t show_out0_ouput(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct pcf8591_data *data = i2c_get_clientdata(to_i2c_client(dev));
	return sprintf(buf, "%d\n", data->aout * 10);
}

static ssize_t set_out0_output(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned int value;
	struct i2c_client *client = to_i2c_client(dev);
	struct pcf8591_data *data = i2c_get_clientdata(client);
	if ((value = (simple_strtoul(buf, NULL, 10) + 5) / 10) <= 255) {
		data->aout = value;
		i2c_smbus_write_byte_data(client, data->control, data->aout);
		return count;
	}
	return -EINVAL;
}

static DEVICE_ATTR(out0_output, S_IWUSR | S_IRUGO, 
		   show_out0_ouput, set_out0_output);

static ssize_t show_out0_enable(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct pcf8591_data *data = i2c_get_clientdata(to_i2c_client(dev));
	return sprintf(buf, "%u\n", !(!(data->control & PCF8591_CONTROL_AOEF)));
}

static ssize_t set_out0_enable(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct pcf8591_data *data = i2c_get_clientdata(client);
	unsigned long val = simple_strtoul(buf, NULL, 10);

	mutex_lock(&data->update_lock);
	if (val)
		data->control |= PCF8591_CONTROL_AOEF;
	else
		data->control &= ~PCF8591_CONTROL_AOEF;
	i2c_smbus_write_byte(client, data->control);
	mutex_unlock(&data->update_lock);
	return count;
}

static DEVICE_ATTR(out0_enable, S_IWUSR | S_IRUGO, 
		   show_out0_enable, set_out0_enable);

static struct attribute *pcf8591_attributes[] = {
	&dev_attr_out0_enable.attr,
	&dev_attr_out0_output.attr,
	&dev_attr_in0_input.attr,
	&dev_attr_in1_input.attr,
	NULL
};

static const struct attribute_group pcf8591_attr_group = {
	.attrs = pcf8591_attributes,
};

static struct attribute *pcf8591_attributes_opt[] = {
	&dev_attr_in2_input.attr,
	&dev_attr_in3_input.attr,
	NULL
};

static const struct attribute_group pcf8591_attr_group_opt = {
	.attrs = pcf8591_attributes_opt,
};


/* Return 0 if detection is successful, -ENODEV otherwise */
static int pcf8591_detect(struct i2c_client *client, int kind,
			  struct i2c_board_info *info)
{
	struct i2c_adapter *adapter = client->adapter;

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE
				     | I2C_FUNC_SMBUS_WRITE_BYTE_DATA))
		return -ENODEV;

	/* Now, we would do the remaining detection. But the PCF8591 is plainly
	   impossible to detect! Stupid chip. */

	strlcpy(info->type, "pcf8591", I2C_NAME_SIZE);

	return 0;
}

static int pcf8591_probe(struct i2c_client *client,
			 const struct i2c_device_id *id)
{
	struct pcf8591_data *data;
	int err;

	if (!(data = kzalloc(sizeof(struct pcf8591_data), GFP_KERNEL))) {
		err = -ENOMEM;
		goto exit;
	}
	
	i2c_set_clientdata(client, data);
	mutex_init(&data->update_lock);

	/* Initialize the PCF8591 chip */
	pcf8591_init_client(client);

	/* Register sysfs hooks */
	err = sysfs_create_group(&client->dev.kobj, &pcf8591_attr_group);
	if (err)
		goto exit_kfree;

	/* Register input2 if not in "two differential inputs" mode */
	if (input_mode != 3) {
		if ((err = device_create_file(&client->dev,
					      &dev_attr_in2_input)))
			goto exit_sysfs_remove;
	}

	/* Register input3 only in "four single ended inputs" mode */
	if (input_mode == 0) {
		if ((err = device_create_file(&client->dev,
					      &dev_attr_in3_input)))
			goto exit_sysfs_remove;
	}

	return 0;

exit_sysfs_remove:
	sysfs_remove_group(&client->dev.kobj, &pcf8591_attr_group_opt);
	sysfs_remove_group(&client->dev.kobj, &pcf8591_attr_group);
exit_kfree:
	kfree(data);
exit:
	return err;
}

static int pcf8591_remove(struct i2c_client *client)
{
	sysfs_remove_group(&client->dev.kobj, &pcf8591_attr_group_opt);
	sysfs_remove_group(&client->dev.kobj, &pcf8591_attr_group);
	kfree(i2c_get_clientdata(client));
	return 0;
}

/* Called when we have found a new PCF8591. */
static void pcf8591_init_client(struct i2c_client *client)
{
	struct pcf8591_data *data = i2c_get_clientdata(client);
	data->control = PCF8591_INIT_CONTROL;
	data->aout = PCF8591_INIT_AOUT;

	i2c_smbus_write_byte_data(client, data->control, data->aout);
	
	/* The first byte transmitted contains the conversion code of the 
	   previous read cycle. FLUSH IT! */
	i2c_smbus_read_byte(client);
}

static int pcf8591_read_channel(struct device *dev, int channel)
{
	u8 value;
	struct i2c_client *client = to_i2c_client(dev);
	struct pcf8591_data *data = i2c_get_clientdata(client);

	mutex_lock(&data->update_lock);

	if ((data->control & PCF8591_CONTROL_AICH_MASK) != channel) {
		data->control = (data->control & ~PCF8591_CONTROL_AICH_MASK)
			      | channel;
		i2c_smbus_write_byte(client, data->control);
	
		/* The first byte transmitted contains the conversion code of 
		   the previous read cycle. FLUSH IT! */
		i2c_smbus_read_byte(client);
	}
	value = i2c_smbus_read_byte(client);

	mutex_unlock(&data->update_lock);

	if ((channel == 2 && input_mode == 2) ||
	    (channel != 3 && (input_mode == 1 || input_mode == 3)))
		return (10 * REG_TO_SIGNED(value));
	else
		return (10 * value);
}

static const struct i2c_device_id pcf8591_id[] = {
	{ "pcf8591", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, pcf8591_id);

static struct i2c_driver pcf8591_driver = {
	.driver = {
		.name	= "pcf8591",
	},
	.probe		= pcf8591_probe,
	.remove		= pcf8591_remove,
	.id_table	= pcf8591_id,

	.class		= I2C_CLASS_HWMON,	/* Nearest choice */
	.detect		= pcf8591_detect,
	.address_data	= &addr_data,
};

static int __init pcf8591_init(void)
{
	if (input_mode < 0 || input_mode > 3) {
		printk(KERN_WARNING "pcf8591: invalid input_mode (%d)\n",
		       input_mode);
		input_mode = 0;
	}
	return i2c_add_driver(&pcf8591_driver);
}

static void __exit pcf8591_exit(void)
{
	i2c_del_driver(&pcf8591_driver);
}

MODULE_AUTHOR("Aurelien Jarno <aurelien@aurel32.net>");
MODULE_DESCRIPTION("PCF8591 driver");
MODULE_LICENSE("GPL");

module_init(pcf8591_init);
module_exit(pcf8591_exit);
