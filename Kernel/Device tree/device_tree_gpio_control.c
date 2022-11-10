#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/gpio/consumer.h>//For gpio functions
#include <linux/delay.h> //For msleep()
#include <linux/kthread.h>

#define DELAY   1000

MODULE_LICENSE("GPL");

struct gpio_desc *gpio_2, *gpio_3, *gpio_4;
struct task_struct *control_led_thread;

static int dt_probe(struct platform_device *pdev);
static int dt_remove(struct platform_device *pdev);

static struct of_device_id my_driver_ids[] = {
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

static int dt_probe(struct platform_device *pdev) {
	struct device *dev = &pdev->dev;

	//Get struct gpio_desc *gpio
	gpio_2 = gpiod_get_index(dev, "led", 0, GPIOD_OUT_HIGH);
	gpio_3 = gpiod_get_index(dev, "led", 1, GPIOD_OUT_HIGH);
	gpio_4 = gpiod_get_index(dev, "led", 2, GPIOD_OUT_HIGH);
   
   	gpiod_put(gpio_2);
	gpiod_put(gpio_3);
	gpiod_put(gpio_4);

	//Set direction for struct gpio_desc *gpio
	gpiod_direction_output(gpio_2, 1);	
	gpiod_direction_output(gpio_3, 1);	
	gpiod_direction_output(gpio_4, 1);

	return 0;
}

static int dt_remove(struct platform_device *pdev) {
	printk("Platform driver removed\n");
	return 0;
}

int control_led_function(void *kernel_data) {
	while(!kthread_should_stop()){
		gpiod_set_value(gpio_2, 1);	
		msleep(DELAY);
		gpiod_set_value(gpio_3, 1);	
		msleep(DELAY);
		gpiod_set_value(gpio_4, 1);	
		msleep(DELAY);

		gpiod_set_value(gpio_4, 0);	
		msleep(DELAY);
		gpiod_set_value(gpio_3, 0);	
		msleep(DELAY);
		gpiod_set_value(gpio_2, 0);	
		msleep(DELAY);
	}
	return 0;
}

int init_module(void)
{
    printk(KERN_INFO "Platform driver parsing device tree node properties\n");
    if(platform_driver_register(&platform_device_driver)) {
		printk("dt_probe - Error! Could not load driver\n");
		return -1;
	}

	control_led_thread = kthread_create(control_led_function, NULL, "Control LED thread");
	if(control_led_thread != NULL){
		wake_up_process(control_led_thread);
		printk("control_led_thread was created and is running now!\n");
	}
	else {
		printk("control_led_thread could not be created!\n");
		return -1;
	}

	return 0;
}

void cleanup_module(void)
{
	kthread_stop(control_led_thread);
	platform_driver_unregister(&platform_device_driver);
}