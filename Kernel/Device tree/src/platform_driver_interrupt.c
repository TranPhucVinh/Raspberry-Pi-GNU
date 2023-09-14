#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#define IRQ_INDEX   0//IRQ index 1 will get the second IRQ num specified in interrupts fields

#define THREAD_FN           NULL
#define INTERRUPT_NAME      "GPIO_3_IRQ"
#define INTERRUPT_ID    	"DEV_ID" //DEV_ID must not be NULL, if using NULL for DEv_ID, request_irq() will fail

#define	IRQ_GPIO			3

MODULE_LICENSE("GPL");

static int dt_probe(struct platform_device *pdev);
static int dt_remove(struct platform_device *pdev);

int irq_number;
int irq_times = 0;

//Must have a driver information for platform device to run probe function
static struct of_device_id my_driver_ids[] = {
	//Must have { /* sentinel */ } at the end of struct of_device_id 
	{
		.compatible = "compatible_string",
	}, { /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, my_driver_ids);

static struct platform_driver platform_device_driver = {
	.probe = dt_probe,
	.remove = dt_remove,
	.driver = {
		.name = "my_device_driver",
		.of_match_table = my_driver_ids,
	},
};

struct device *dev;

irq_handler_t irq_handler(unsigned int irq, void* dev_id, struct pt_regs *regs){
    irq_times += 1;
	printk("Device ID %s; interrupt occured %d times\n", (char*)dev_id, irq_times);
    return (irq_handler_t) IRQ_HANDLED;
}

static int dt_probe(struct platform_device *pdev) {
	// As using GPIO for interrupt, i.e external interrupt, that GPIO must be input
	if(gpio_direction_input(IRQ_GPIO)){
	  printk("Unable to set GPIO %d to input\n", IRQ_GPIO);
	}

    dev = &pdev->dev;
    irq_number = platform_get_irq(pdev, IRQ_INDEX);
    if (irq_number < 0) printk("Unable to get IRQ number in platform device\n");
    else printk("IRQ number is %d\n", irq_number);

	if (devm_request_threaded_irq(dev, irq_number, (irq_handler_t) irq_handler, (irq_handler_t) THREAD_FN, IRQF_TRIGGER_RISING|IRQF_SHARED, INTERRUPT_NAME, INTERRUPT_ID) != 0){
        printk("Can't request interrupt number %d\n", irq_number);
        /*
            Return any error to userspace if fail to request_irq()
            E.g: ENXIO No such device or address
        */
        return -ENXIO;
    } else printk("Request interrupt number %d successfully\n", irq_number);

	return 0;
}

static int dt_remove(struct platform_device *pdev) {
	printk("dt_remove\n");
	return 0;
}

int init_module(void)
{
    printk(KERN_INFO "Platform driver parsing device tree node properties\n");
    if(platform_driver_register(&platform_device_driver)) {
		printk("dt_probe - Error! Could not load driver\n");
		return -1;
	}
	return 0;
}

void cleanup_module(void)
{
    devm_free_irq(dev, irq_number, INTERRUPT_ID);
	platform_driver_unregister(&platform_device_driver);
}