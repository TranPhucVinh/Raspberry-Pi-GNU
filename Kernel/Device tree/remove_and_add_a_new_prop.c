#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/slab.h> //for kmalloc()

#define COMPATIBLE    		"gpio-restart"
#define OLD_PROP			"priority"
#define NEW_PROP			"new_priority"

DEFINE_MUTEX(of_mutex);
MODULE_LICENSE("GPL");

struct of_changeset *ocs;

int remove_property_and_add_new(struct of_changeset *ocs, const char* compatible, const char *old_prop_val, const char *new_prop_val){
	struct device_node 	*dev_node;
	struct property  	*old_prop, *new_prop;

	old_prop = kmalloc(sizeof(struct property), GFP_KERNEL);

	dev_node = kmalloc(sizeof(struct device_node), GFP_KERNEL);
	dev_node = of_find_compatible_node(NULL, NULL, compatible);

	old_prop = of_find_property(dev_node, old_prop_val, NULL);

	of_changeset_init(ocs);//Init of_changset objects

	of_changeset_remove_property(ocs, dev_node, old_prop);
	
	new_prop = kmalloc(sizeof(struct property), GFP_KERNEL);
	new_prop->name = kstrdup(new_prop_val, GFP_KERNEL);
	new_prop->value = NULL;//property value
	new_prop->length = 0;
	of_changeset_add_property(ocs, dev_node, new_prop);	

	mutex_lock(&of_mutex);
	int ret = of_changeset_apply(ocs);
	
	if(ret < 0) {
		printk("Unable to update device tree property, ret: %d\n", ret);
		return -1;
	} else printk("Update device tree property successfully, ret: %d\n", ret);

	mutex_unlock(&of_mutex);	
	return 0; 
}
void revert_to_origin_device_tree(struct of_changeset *ocs){
	of_changeset_revert(ocs);
	return;
}
int init_module(void)
{
	mutex_init(&of_mutex);
    ocs = kmalloc(sizeof(struct of_changeset), GFP_KERNEL);
	remove_property_and_add_new(ocs, COMPATIBLE, OLD_PROP, NEW_PROP);	
	return 0;
}

void cleanup_module(void)
{
    revert_to_origin_device_tree(ocs);
	printk(KERN_INFO "clean up module\n");
}
