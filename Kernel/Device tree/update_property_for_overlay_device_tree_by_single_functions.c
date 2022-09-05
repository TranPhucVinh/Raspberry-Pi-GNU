#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

struct device_node  *dev_node;
struct of_changeset *ocs;
struct property  	*prop;

struct mutex 		of_mutex;

int init_module(void)
{
	//Find the overlay device node with device_type "new_dt_node"
	dev_node = of_find_node_by_type(NULL, "new_dt_node");
	
	ocs = kmalloc(sizeof(struct of_changeset*), GFP_KERNEL);
	of_changeset_init(ocs);

	prop = kmalloc(sizeof(struct property*), GFP_KERNEL);
	prop->name = kstrdup("compatible", GFP_KERNEL);//property name
	prop->value = kstrdup("new_compatible_string", GFP_KERNEL);//property value
	prop->length = strlen(prop->value) + 1;

	//Update the "compatible" property
	of_changeset_update_property(ocs, dev_node, prop);

	mutex_lock(&of_mutex);
	int ret = of_changeset_apply(ocs);
	if(ret < 0) {
		printk("Unable to update device tree property, ret: %d\n", ret);
		return -1;
	}
	mutex_unlock(&of_mutex);

	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "clean up module\n");
}
