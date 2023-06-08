# of_find_property()
``of_find_property()`` can only be used to read string value of a property:

```c
fixedregulator@0 {
	compatible = "regulator-fixed";
	phandle = <0x2>;
	regulator-name = "VCCPINT";
};
```

```c
#define COMPATIBLE    	"regulator-fixed"
#define PROP			"regulator-name"

MODULE_LICENSE("GPL");
int init_module(void)
{
	struct device_node  *dev_node;
	struct property  	*prop;

	dev_node = of_find_compatible_node(NULL, NULL, COMPATIBLE);
	prop = of_find_property(dev_node, PROP, NULL);
	printk("%s %s\n", prop->name, prop->value);//regulator-name VCCPINT
	return 0;
}
```
``of_find_property()`` can't be used to read int value of a property, like ``phandle = <0x2>``, as this property is treated as an [array](README.md#array). Reading it by ``of_find_property()`` results in garbage value, use [of_property_read_u32_array()](API.md#of_property_read_u32_array) instead.
# of_find_compatible_node() to find a node device with compatible string ``COMPATIBLE``

```c
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
	}
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "clean up module\n");
}
```
# of_find_compatible_node() to find all nodes with compatible string ``COMPATIBLE`` by using recursive

To find all nodes with the compatible string, [recursive function](https://github.com/TranPhucVinh/C/blob/master/Algorithms/Function/Recursive%20function.md) must be used:

```c
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

	// Setting with kzalloc() won't set dev_node to NULL
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
```
# of_property_read_u32_array()

Read ``interrupts`` properties from node ``new_dt_node``

```sh
new_dt_node {
        compatible = "compatible_string";     
        interrupts = <0x0 0xa 0x4 0x0 0xb 0x4 0x0 0xc 0x4>;
};
```

```c
#define ARR_SIZE	9

int int_array[ARR_SIZE];
struct device_node  *dev_node;
int init_module(void)
{
	int i = 0;
	dev_node = (struct device_node *)kmalloc(sizeof(struct device_node), GFP_KERNEL);
	dev_node = of_find_compatible_node(NULL, NULL, "compatible_string");

	int ret = of_property_read_u32_array(dev_node, "interrupts", int_array, ARR_SIZE);
	printk("ret %d\n", ret);
	for (i = 0; i < ARR_SIZE; i++){
		printk("%d \n", int_array[i]);
	}
	printk("\n");
	return 0;
}
```

Note that the ``size`` parameter to read must be fixed to the existed size of the property. E.g: As ``interrupts`` array size is ``9`` but use array size ``100`` to read, then there will be error ``EOVERFLOW`` (``75``).

# for_each_child_of_node()
Find a node with compatile string ``simple-bus`` and print out the ``name`` and ``full_name`` of all of its child nodes:
```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/slab.h> //for kmalloc()

MODULE_LICENSE("GPL");

struct device_node *parent_node, *child_node;

int init_module(void)
{
	parent_node = (struct device_node *)kmalloc(sizeof(struct device_node), GFP_KERNEL);
	child_node = (struct device_node *)kmalloc(sizeof(struct device_node), GFP_KERNEL);
	parent_node = of_find_compatible_node(NULL, NULL, "simple-bus");
	for_each_child_of_node(parent_node, child_node){
		printk("name: %s\n", child_node->name);
		printk("full_name: %s\n", child_node->full_name);
	}

	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "clean up module\n");
}
```