#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/io.h>

#define BCM2837_PHY_ADDR                0x3f000000 //BCM2837 physical address start
#define GPIO_BASE                       (BCM2837_PHY_ADDR + 0x200000) /* GPIO controller */

#define LED     3
#define BUTTON  2

#define DELAY       100

#define MAPPING_SIZE    0x100 //MAPPING_SIZE should be a valid value and big enough to avoid memory crashing in ioremap()

MODULE_LICENSE("GPL");

struct 	task_struct *kthread_1;

unsigned int *gpio_base;

unsigned int *led_address;
unsigned int *led_on_address;
unsigned int *led_off_address;

unsigned int *button_address;
unsigned int *read_address;

unsigned long led_output = 0;
unsigned long button_input = 0;

unsigned long led_on_value = 0;//led_on_value to write ON to LED
unsigned long led_off_value = 0;//led_off_value to write OFF to LED

int thread_function(void *kernel_data) {
	while(!kthread_should_stop()){
		int status = (ioread32(read_address)>>BUTTON)&1;

		if (status == 1){
            iowrite32(led_on_value, led_on_address);
        } else if (status == 0){
            iowrite32(led_off_value, led_off_address);
        }
        msleep(DELAY);
    }
	return 0;
}

int init_module(void)
{
	//Memory mapping
	gpio_base = (unsigned int *)ioremap(GPIO_BASE, MAPPING_SIZE);
	if (gpio_base == NULL) printk("Couldn't perform mapping\n");
	else {
		led_address = gpio_base + LED/10; //Set virtual address for LED pin
        button_address = gpio_base + BUTTON/10; //Set virtual address for LED pin

        //Set LED as output
        led_output = ioread32(led_address);
		led_output  &= ~(7<<(((LED)%10)*3));
		led_output |= (1<<(((LED)%10)*3));
        iowrite32(led_output, led_address);

        //Set BUTTON as input
        button_input = ioread32(button_address);
        button_input &= ~(7<<(((BUTTON)%10)*3));
        iowrite32(button_input, button_address);

		led_on_address = gpio_base + 7; //Set virtual address for ON address
		led_off_address = gpio_base + 10; //Set virtual address for OFF address
		
		led_on_value = 1<<LED;	//set value to write ON for LED
		led_off_value = led_on_value;  //set value to write OFF for LED
		
		read_address =  gpio_base+13; //GET GPIO value in GPLEV0

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
