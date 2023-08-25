#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/i2c.h>
#include <linux/kernel.h>

#define TOTAL_MINOR                1
#define BASE_MINOR				   0

#define DEVICE_NAME  			"PCF8574_DRIVER" //Character device to handle PCF8574
#define DEVICE_CLASS 			"PCF8574_CLASS"

#define I2C_BUS_AVAILABLE		1			/* /dev/i2c-1 */
#define SLAVE_DEVICE_NAME		"PCF8574"	/* Device and Driver Name */
#define PCF8574_SLAVE_ADDRESS	0x20		/* PCF8574 I2C address */

MODULE_LICENSE("GPL");

static struct i2c_adapter *pcf8574_i2c_adapter = NULL;
static struct i2c_client  *pcf8574_i2c_client = NULL;

static const struct i2c_device_id pcf8574_id[] = {
		{ SLAVE_DEVICE_NAME, 0 }, 
		{ }
};

static struct i2c_driver pcf8574_driver = {
	.driver = {
		.name = SLAVE_DEVICE_NAME,
		.owner = THIS_MODULE
	}
};

static struct i2c_board_info pcf8574_i2c_board_info = {
	I2C_BOARD_INFO(SLAVE_DEVICE_NAME, PCF8574_SLAVE_ADDRESS)
};

dev_t dev_id;

struct cdev *character_device;
struct class *device_class;
struct device *device;

int dev_open(struct inode *, struct file *);
int dev_close(struct inode *, struct file *);
ssize_t dev_read(struct file*, char __user *, size_t, loff_t *);
ssize_t dev_write(struct file *, const char __user *, size_t, loff_t *);

struct file_operations fops = {
	.open = dev_open,
	.release = dev_close,
	.read = dev_read,
	.write = dev_write,
};

int dev_open(struct inode *inodep, struct file *filep)
{
	printk("open\n");
	return 0;
}

int dev_close(struct inode *inodep, struct file *filep)
{
	printk("close\n");
	return 0;
}

ssize_t dev_read(struct file*filep, char __user *buf, size_t len, loff_t *offset)
{
	printk("read\n");
	return 0;
}

char data[100];
ssize_t dev_write(struct file*filep, const char __user *buf, size_t len, loff_t *offset)
{
	memset(data, 0, sizeof(data));
	copy_from_user(data, buf, len);
	printk("write data: %s\n", data);
	return sizeof(data);
}

int device_init(void)
{
	int ret;

	ret = alloc_chrdev_region(&dev_id, BASE_MINOR, TOTAL_MINOR, "fops_alloc_chrdev_region");
	if(ret)
	{
		printk("can not register major no\n");
		return ret;
	}
	printk(KERN_INFO "register successfully major now is: %d\n", MAJOR(dev_id));
	character_device = cdev_alloc();
	character_device->owner = THIS_MODULE;
	character_device->ops = &fops;
	character_device->dev = dev_id;

	cdev_add(character_device, dev_id, 1);
	device_class = class_create(THIS_MODULE, DEVICE_CLASS);
	device = device_create(device_class, NULL, dev_id, NULL, DEVICE_NAME);

	//Add driver for PCF8574
	pcf8574_i2c_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);

	if(pcf8574_i2c_adapter != NULL) {
		pcf8574_i2c_client = i2c_new_client_device(pcf8574_i2c_adapter, &pcf8574_i2c_board_info);
		if(pcf8574_i2c_client != NULL) {
			if(i2c_add_driver(&pcf8574_driver) != -1) ret = 0;
			else printk("Can't add driver...\n");
		}
		i2c_put_adapter(pcf8574_i2c_adapter);
	}
	printk("PCF8574 driver added!\n");

	//Then turn on a specifc GPIO when insmod successfully
	#define WRITE_SIZE	1
	char write_value[WRITE_SIZE] = {0x01};
	i2c_master_send(pcf8574_i2c_client, write_value, WRITE_SIZE);

	return 0;
}

void device_exit(void)
{
	printk("Device remove\n");
	unregister_chrdev_region(dev_id, TOTAL_MINOR); 
	cdev_del(character_device);
	device_destroy(device_class, dev_id);
	class_destroy(device_class);


	i2c_unregister_device(pcf8574_i2c_client);
	i2c_del_driver(&pcf8574_driver);
}

module_init(device_init);
module_exit(device_exit);
