#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

#define LED			3
#define BUTTON		2

#define DEV_NAME    "Device name"
#define DEV_ID      NULL

MODULE_LICENSE("GPL");

int irq_number;
int led_status;

int triggered_times = 0;

irq_handler_t gpio_irq_handler(unsigned int irq, void* dev_id, struct pt_regs *regs){
	led_status = !led_status;
	gpio_direction_output(LED, led_status);

    printk("IRQ triggered times %d\n", triggered_times);
    triggered_times += 1;
    return (irq_handler_t) IRQ_HANDLED;
}

int init_module(void)
{
	printk(KERN_INFO "Hello, World !\n");

    if(gpio_direction_input(BUTTON)){
        printk("Unable to set GPIO %d to input\n", BUTTON);
    }

	irq_number = gpio_to_irq(BUTTON);

    if (request_irq(irq_number, (irq_handler_t) gpio_irq_handler, IRQF_TRIGGER_RISING, DEV_NAME, DEV_ID) != 0){
        printk("Can't request interrupt number %d\n", irq_number);
    } else printk("Request interrupt number %d successfully\n", irq_number);

	return 0;
}

void cleanup_module(void)
{
    /*
        Must have free_irq() function in cleanup (for rmmod command)
        If not having free_irq(), the interrupt still existed in /proc/interrupts after calling rmmod
    */
	free_irq(irq_number, DEV_ID);
	printk(KERN_INFO "clean up module\n");
}