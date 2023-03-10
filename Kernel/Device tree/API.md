# for_each_child_of_node()

``for_each_child_of_node()`` will list out all child nodes inside a parent node. ``for_each_child_of_node()`` is a macro

```c
for_each_child_of_node(parent, child)
```

Find a node with compatile string ``simple-bus`` and print out the ``name`` and ``full_name`` of all of its child nodes.

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
