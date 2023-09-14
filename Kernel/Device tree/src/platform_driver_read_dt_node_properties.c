#include <linux/module.h>
#include <linux/init.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>

#define SIZE	4

MODULE_LICENSE("GPL");

static int dt_probe(struct platform_device *pdev);
static int dt_remove(struct platform_device *pdev);

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

static int dt_probe(struct platform_device *pdev) {
	struct device *dev = &pdev->dev;
    int ret;
    int int_val_1;
	int int_array[SIZE];
	const char *string_val_1, *string_val_2;

	/* Check for device properties */
	if(!device_property_present(dev, "string_val_1")) {
		printk("dt_probe - Error! Device property 'string_val_1' not found!\n");
		return -1;
	}
	if(!device_property_present(dev, "string_val_2")) {
		printk("dt_probe - Error! Device property 'string_val_2' not found!\n");
		return -1;
	}

    if(!device_property_present(dev, "int_val_1")) {
		printk("dt_probe - Error! Device property 'int_val_1' not found!\n");
		return -1;
	}

	/* Read device properties */
	ret = device_property_read_string(dev, "string_val_1", &string_val_1);
	if(ret) {
		printk("Could not read 'string_val_1'\n");
		return -1;
	}
	printk("string_val_1: %s\n", string_val_1);

    ret = device_property_read_string(dev, "string_val_2", &string_val_2);
	if(ret) {
		printk("Could not read 'string_val_2'\n");
		return -1;
	}
	printk("string_val_2: %s\n", string_val_2);

	ret = device_property_read_u32(dev, "int_val_1", &int_val_1);
	if(ret) {
		printk("Could not read 'int_val_1'\n");
		return -1;
	}
	printk("int_val_1: %d\n", int_val_1);

	ret = device_property_read_u32_array(dev, "int_array", int_array, SIZE);
	if(ret) {
		printk("Could not read 'int_array'\n");
		return -1;
	}
	printk("int_array members: %d %d %d %d\n", int_array[0], int_array[1], int_array[2], int_array[3]);
	
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
	platform_driver_unregister(&platform_device_driver);
}
