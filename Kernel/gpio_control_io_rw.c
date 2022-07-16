#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h> //For msleep()
#include <linux/io.h>

#define BCM2837_PHY_ADDR                0x3f000000 //BCM2837 physical address start
#define GPIO_BASE                       (BCM2837_PHY_ADDR + 0x200000) /* GPIO controller */

#define LED			4

#define DELAY                   5000

#define MAPPING_SIZE            0x100 //Map 256 physical registers to virtual registers

MODULE_LICENSE("GPL");

struct 	task_struct *kthread_1;

unsigned int *gpio_base;

unsigned int *led_address;
unsigned int *on_address;
unsigned int *off_address;

unsigned long output_value = 0;//output_value to setup output mode, read, write operation
unsigned long on_value = 0;//on_value to write ON to LED
unsigned long off_value = 0;//off_value to write OFF to LED

int thread_function(void *kernel_data) {
	while(!kthread_should_stop()){
                iowrite32(on_value, on_address);
                msleep(DELAY);
                iowrite32(off_value, off_address);
                msleep(DELAY);
	}
	return 0;
}

int init_module(void)
{
	printk(KERN_INFO "Hello, World !\n");

	//Memory mapping
	gpio_base = (unsigned int *)ioremap(GPIO_BASE, MAPPING_SIZE);
	if (gpio_base == NULL) printk("Couldn't perform mapping\n");
	else {
		led_address = gpio_base + LED/10; //Set virtual address for LED pin
		on_address = gpio_base + 7; //Set virtual address for ON address
		off_address = gpio_base + 10; //Set virtual address for OFF address

		//Set GPIO output
		output_value  &= ~(7<<(((LED)%10)*3));
		output_value |= (1<<(((LED)%10)*3));

		iowrite32(output_value, led_address); //Set LED output
		
		on_value = 1<<LED;	//set value to write ON
		off_value = on_value;   //set value to write OFF
		
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
	}
		
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "clean up module\n");
	iounmap(gpio_base);
	kthread_stop(kthread_1);
}
