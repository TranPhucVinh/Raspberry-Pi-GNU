#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

#define COMPATIBLE    	"compatible_string"
#define TYPE			NULL
#define FROM_NODE		NULL	// Node for of_find_compatible_node() to find from

struct device_node *dev_node;

int init_module(void)
{
	dev_node = kmalloc(sizeof(struct device_node), GFP_KERNEL);
	dev_node = of_find_compatible_node(FROM_NODE, TYPE, COMPATIBLE);

	// There must be error handler with dev_node == NULL for the case 
	// no compatible string is found.
	// When there is no node with compatible string is found, accessing it
	// with dev_node->name and dev_node->full_name cause the board to reset
	if (dev_node == NULL) printk("No node has compatible string %s\n", COMPATIBLE);
	else {
		printk("name: %s\n", dev_node->name);
		printk("full_name: %s\n", dev_node->full_name);

		//Get ref_count of a device tree node
		printk("ref_count: %d\n", atomic_read(&((dev_node->kobj).kref.refcount).refs));

		dev_node = of_node_get(dev_node);//Increase ref_count by 1
		printk("after of_node_get() ref_count: %d\n", atomic_read(&((dev_node->kobj).kref.refcount).refs));
		of_node_put(dev_node);//Decrease ref_count by 1
		printk("after of_node_put() ref_count: %d\n", atomic_read(&((dev_node->kobj).kref.refcount).refs));
	}
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "clean up module\n");
}