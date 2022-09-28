#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h> //For msleep()
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define BCM2837_PHY_ADDR                0x3f000000 //BCM2837 physical address start
#define GPIO_BASE                       (BCM2837_PHY_ADDR + 0x200000) /* GPIO controller */

#define GPIO_OUT(addr,pin) {\
        *(addr+((pin)/10)) &= ~(7<<(((pin)%10)*3));\
        *(addr+((pin)/10)) |= (1<<(((pin)%10)*3));\
}
#define GPIO_SET(addr,pin) *(addr+7)=1<<pin //Set GPIO in GPSET0
#define GPIO_CLR(addr,pin) *(addr+10)=1<<pin //Clear GPIO in GPCLR0

#define LED			3

#define DELAY       1000

#define MAPPING_SIZE    0x100 //MAPPING_SIZE should be a valid value and big enough to avoid memory crashing in ioremap()

//Character device setup
#define DEVICE_NAME					"fops_character_device"
#define DEVICE_CLASS				"fops_device_class"

#define TOTAL_MINOR                1
#define BASE_MINOR				   0

MODULE_LICENSE("GPL");

struct 	task_struct *kthread_1;

unsigned int *gpio_base;

struct chr_dev_info {
	struct cdev *character_device;
	struct class *device_class;
	struct device *device;
	dev_t dev_id;
} dev_info;

int 	dev_open(struct inode *, struct file *);
int 	dev_close(struct inode *, struct file *);
ssize_t dev_read(struct file*, char __user *, size_t, loff_t *);
ssize_t dev_write(struct file *, const char __user *, size_t, loff_t *);
int 	create_character_device(const char* dev_name, const char* dev_class, int total_minor, int base_minor, struct chr_dev_info *dev_info, struct file_operations *fops);
void 	destroy_character_device(struct chr_dev_info *dev_info, int total_minor);

struct file_operations fops = {
	.open = dev_open,
	.release = dev_close,
	.read = dev_read,
	.write = dev_write,
};

int thread_function(void *kernel_data) {
	while(!kthread_should_stop()){
        GPIO_SET(gpio_base, LED);
        msleep(DELAY);
        GPIO_CLR(gpio_base, LED);
        msleep(DELAY);
	}
	return 0;
}

int init_module(void)
{
	printk(KERN_INFO "Hello, World !\n");

	if (create_character_device(DEVICE_NAME, DEVICE_CLASS, TOTAL_MINOR, BASE_MINOR, &dev_info, &fops)){
		printk("Unable to create character device %s\n", DEVICE_NAME);
		return -1;
	}

    //Memory mapping
    gpio_base = (unsigned int *)devm_ioremap(dev_info.device, GPIO_BASE, MAPPING_SIZE);
    if (gpio_base == NULL) printk("Couldn't perform mapping\n");
    else {
		//Thread setup
		kthread_1 = kthread_create(thread_function, NULL, "kthread_1");
		if(kthread_1 != NULL){
			wake_up_process(kthread_1); //Start kthread_1
			printk("kthread_1 was created and is running now!\n");
		}
		else {
			printk("kthread_1 could not be created!\n");
			return -1;
		}
		GPIO_OUT(gpio_base, LED);
	}

	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "clean up module\n");
	printk("Device %s remove\n", DEVICE_NAME);
	destroy_character_device(&dev_info, TOTAL_MINOR);

	devm_iounmap(dev_info.device, gpio_base);
	kthread_stop(kthread_1);
}

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

ssize_t dev_write(struct file*filep, const char __user *buf, size_t len, loff_t *offset)
{
	printk("write\n");
	return 0;
}

int create_character_device(const char* dev_name, const char* dev_class, int total_minor, int base_minor, struct chr_dev_info *dev_info, struct file_operations *fops){
	int ret;

	ret = alloc_chrdev_region(&(dev_info->dev_id), base_minor, total_minor, "fops_alloc_chrdev_region");
	if(ret)
	{
		printk("can not register major no\n");
		return ret;
	}
	printk(KERN_INFO "register successfully major now is: %d\n", MAJOR(dev_info->dev_id));
	dev_info->character_device = cdev_alloc();
	dev_info->character_device->owner = THIS_MODULE;
	dev_info->character_device->ops = fops;
	dev_info->character_device->dev = dev_info->dev_id;

	cdev_add(dev_info->character_device, dev_info->dev_id, 1);
	dev_info->device_class = class_create(THIS_MODULE, dev_class);
	dev_info->device = device_create(dev_info->device_class, NULL, dev_info->dev_id, NULL, dev_name);

	return 0;
}

void destroy_character_device(struct chr_dev_info *dev_info, int total_minor){
	unregister_chrdev_region(dev_info->dev_id, total_minor); 
	cdev_del(dev_info->character_device);
	device_destroy(dev_info->device_class, dev_info->dev_id);
	class_destroy(dev_info->device_class);
}
