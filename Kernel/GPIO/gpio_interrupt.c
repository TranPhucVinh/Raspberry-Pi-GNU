#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/kthread.h>
#include <linux/delay.h> //For msleep()

#define DEV_NAME    "GPIO_IRQ"
#define DEV_ID    	"DEV_ID" //DEV_ID must not be NULL, if using NULL for DEv_ID, request_irq() will fail
#define GPIO		2
#define LABEL       "GPIO 2"//GPIO label to be displayed in sysfs

#define DELAY   1000

MODULE_LICENSE("GPL");

struct task_struct *kthread_1;

int irq_times = 0;
int irq_number;
int led_status = 0;

int thread_function(void *kernel_data) {
	while(!kthread_should_stop()){
        led_status = !led_status;
        gpio_direction_output(GPIO, led_status);
        msleep(DELAY);
	}
	return 0;
}

irq_handler_t irq_1_handler(unsigned int irq, void* dev_id, struct pt_regs *regs){
	printk("Device ID %s; interrupt occured %d times\n", (char*)dev_id, irq_times);
	irq_times += 1;
    return (irq_handler_t) IRQ_HANDLED;
}

int init_module(void)
{
	printk(KERN_INFO "Hello, World !\n");

    //Allocate GPIO for output mode and set HIGH level
    gpio_direction_output(GPIO, 1);

    irq_number = gpio_to_irq(GPIO);
    if (request_irq(irq_number, (irq_handler_t) irq_1_handler, IRQF_TRIGGER_RISING|IRQF_SHARED, DEV_NAME, DEV_ID) != 0){
        printk("Can't request interrupt number %d\n", irq_number);

        /*
            Return any error to userspace if fail to request_irq()
            E.g: ENXIO No such device or address
        */
        return -ENXIO;
    } else printk("Request interrupt number %d successfully\n", irq_number);

    kthread_1 = kthread_create(thread_function, NULL, "kthread_1");
	if(kthread_1 != NULL){
		wake_up_process(kthread_1); //Start kthread_1
		printk("kthread_1 was created and is running now!\n");
	}
	else {
		printk("kthread_1 could not be created!\n");
		return -ENXIO;
	}

	return 0;
}

void cleanup_module(void)
{
    /*
        Must have free_irq() function in cleanup (for rmmod command)
        If not having free_irq(), the interrupt still existed in /proc/interrupts after calling rmmod
    */
	free_irq(irq_number, DEV_ID);
    kthread_stop(kthread_1);
	printk(KERN_INFO "clean up module\n");
}
