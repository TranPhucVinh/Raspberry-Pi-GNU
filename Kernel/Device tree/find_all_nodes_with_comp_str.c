#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

#define COMPATIBLE    	"COMP_STR"

struct device_node *dev_node;

void get_all_nodes_with_comp_str(struct device_node *dev_node, const char *comp_str);

int init_module(void)
{
	dev_node = kzalloc(sizeof(struct device_node), GFP_KERNEL);

	// Setting with kzallo() won't set dev_node to NULL
	// so set dev_node to NULL for recursive call
	dev_node = NULL;
	printk("Init is empty now\n");
	get_all_nodes_with_comp_str(dev_node, COMPATIBLE);
	
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "clean up module\n");
}

void get_all_nodes_with_comp_str(struct device_node *dev_node, const char *comp_str){
	static int total_node_found = 0;

	dev_node = of_find_compatible_node(dev_node, NULL, comp_str);

	if (dev_node == NULL) {
		if (!total_node_found) printk("No node has compatible string %s\n", comp_str);
		else return;
	}
	else {
		printk("name: %s\n", dev_node->name);
		printk("full_name: %s\n", dev_node->full_name);
		total_node_found += 1;
		get_all_nodes_with_comp_str(dev_node, comp_str);
	}
}