#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/gpio.h>

#define DELAY 		100
#define NO_USE_FLAG	0
#define LED			3
#define BUTTON		2

MODULE_LICENSE("GPL");

struct 	timer_list kernel_timer;

void timer_callback(struct timer_list* tl){
	gpio_direction_output(LED, gpio_get_value(BUTTON));
	mod_timer(&kernel_timer, jiffies + msecs_to_jiffies(DELAY));
}

int init_module(void)
{
	printk(KERN_INFO "Hello, World !\n");

	if(gpio_request(BUTTON, "No label")){
		printk("Can't allocate GPIO %d\n", BUTTON);
	}
	
	if(gpio_direction_input(BUTTON)){
		printk("Can't set GPIO %d to output\n", BUTTON);
	}

		
	timer_setup(&kernel_timer, timer_callback, NO_USE_FLAG);

	kernel_timer.expires = jiffies + msecs_to_jiffies(DELAY); //Delay at DELAY seconds for the 1st time

	add_timer(&kernel_timer);
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "clean up module\n");
	del_timer(&kernel_timer);
}
