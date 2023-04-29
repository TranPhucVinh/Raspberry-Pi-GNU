#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#define THREAD_FN           NULL
#define INTERRUPT_NAME      "GPIO_IRQ"
#define INTERRUPT_ID    	"INTERRUPT_ID" //DEV_ID must not be NULL, if using NULL for DEv_ID, request_irq() will fail

#define	IRQ_GPIO_2			2
#define	IRQ_GPIO_3			3

MODULE_LICENSE("GPL");

static int dt_probe(struct platform_device *pdev);
static int dt_remove(struct platform_device *pdev);

int gpio_2_irq, gpio_3_irq;
int gpio_2_irq_times = 0, gpio_3_irq_times = 0;

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

irq_handler_t gio_2_irq_handler(unsigned int irq, void* dev_id, struct pt_regs *regs){
    gpio_2_irq_times += 1;
	printk("GPIO %d has interrupt occured %d times\n", IRQ_GPIO_2, gpio_2_irq_times);
    return (irq_handler_t) IRQ_HANDLED;
}

irq_handler_t gio_3_irq_handler(unsigned int irq, void* dev_id, struct pt_regs *regs){
    gpio_3_irq_times += 1;
	printk("GPIO %d has interrupt occured %d times\n", IRQ_GPIO_3, gpio_3_irq_times);
    return (irq_handler_t) IRQ_HANDLED;
}

static int dt_probe(struct platform_device *pdev) {
    dev = &pdev->dev;

	if(gpio_direction_input(IRQ_GPIO_2)){
	  printk("Unable to set GPIO %d to input\n", IRQ_GPIO_2);
	}
	gpio_2_irq = platform_get_irq_byname(pdev, "gpio_2_irq");
    if (gpio_2_irq < 0) printk("Unable to get GPIO 2 IRQ in platform device\n");
    else printk("GPIO 2 IRQ number is %d\n", gpio_2_irq);

	if (devm_request_threaded_irq(dev, gpio_2_irq, (irq_handler_t) gio_2_irq_handler, (irq_handler_t) THREAD_FN, IRQF_TRIGGER_RISING|IRQF_SHARED, INTERRUPT_NAME, INTERRUPT_ID) != 0){
        printk("Can't request interrupt number %d\n", gpio_2_irq);
        return -ENXIO;
    } else printk("Request interrupt number %d successfully\n", gpio_2_irq);

	if(gpio_direction_input(IRQ_GPIO_3)){
	  printk("Unable to set GPIO %d to input\n", IRQ_GPIO_3);
	}
	gpio_3_irq = platform_get_irq_byname(pdev, "gpio_3_irq");
    if (gpio_3_irq < 0) printk("Unable to get GPIO 3 IRQ in platform device\n");
    else printk("GPIO 3 IRQ number is %d\n", gpio_3_irq);

	if (devm_request_threaded_irq(dev, gpio_3_irq, (irq_handler_t) gio_3_irq_handler, (irq_handler_t) THREAD_FN, IRQF_TRIGGER_RISING|IRQF_SHARED, INTERRUPT_NAME, INTERRUPT_ID) != 0){
        printk("Can't request interrupt number %d\n", gpio_3_irq);
        return -ENXIO;
    } else printk("Request interrupt number %d successfully\n", gpio_3_irq);

	return 0;
}

static int dt_remove(struct platform_device *pdev) {
	printk("dt_remove\n");
	return 0;
}

int init_module(void)
{
    if(platform_driver_register(&platform_device_driver)) {
		printk("dt_probe - Error! Could not load driver\n");
		return -1;
	}
	return 0;
}

void cleanup_module(void)
{
    devm_free_irq(dev, gpio_2_irq, INTERRUPT_ID);
	devm_free_irq(dev, gpio_3_irq, INTERRUPT_ID);
	platform_driver_unregister(&platform_device_driver);
}