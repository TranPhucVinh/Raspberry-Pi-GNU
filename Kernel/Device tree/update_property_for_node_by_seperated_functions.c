#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/slab.h>

#define NODE_TYPE    "new_dt_node"
#define PROP_VAL     "compatible"
#define UPDATE_VAL   "new_compatible_string"

DEFINE_MUTEX(of_mutex);
MODULE_LICENSE("GPL");

struct of_changeset *ocs;

/**
 * update_node_properties - Update device tree node properties
 * @ocs:
 * @node_type:
 * @property_name:
 * @update_value: pointer to the value to update, use type const char as using const to update
 * properties in this specific case, change to other type when inheriting this function
 */
int update_node_properties(struct of_changeset *ocs, const char* node_type, const char* property_name, const char *update_value){
	struct device_node  *dev_node;
	struct property  	*prop;

	dev_node = of_find_node_by_type(NULL, node_type);
	
	//Init of_changset objects
	of_changeset_init(ocs);

	prop = kmalloc(sizeof(struct property), GFP_KERNEL);
	prop->name = kstrdup(property_name, GFP_KERNEL);//property name
	prop->value = kstrdup(update_value, GFP_KERNEL);//property value
	prop->length = strlen(prop->value) + 1;

	of_changeset_update_property(ocs, dev_node, prop);
	
	mutex_lock(&of_mutex);
	int ret = of_changeset_apply(ocs);
	if(ret < 0) {
		printk("Unable to update device tree property, ret: %d\n", ret);
		return -1;
	}
	mutex_unlock(&of_mutex);
	
	/*
		Must not use kfree() for prop inside this function, as prop must still be available after calling update_node_properties in init_module so that this will take effect on the device tree.

		Calling kfree(prop) will cause the OS to freeze
	*/

	return 0; 
}

/**
 * revert_to_origrin_device_tree - Revert to original device tree setup
 * @ocs:
 */
void revert_to_origrin_device_tree(struct of_changeset *ocs){
	of_changeset_revert(ocs);
	return;
}

int init_module(void)
{
	mutex_init(&of_mutex);
    ocs = kmalloc(sizeof(struct of_changeset), GFP_KERNEL);
	update_node_properties(ocs, NODE_TYPE, PROP_VAL, UPDATE_VAL);	
	return 0;
}

void cleanup_module(void)
{
    revert_to_origrin_device_tree(ocs);
	printk(KERN_INFO "clean up module\n");
}
