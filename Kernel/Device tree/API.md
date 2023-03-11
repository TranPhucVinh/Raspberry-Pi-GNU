# struct

```c
#include <linux/of.h>
```

```c
struct device_node {
	const char *name;
	phandle phandle;
	const char *full_name;
	struct fwnode_handle fwnode;

	struct	property *properties;
	struct	property *deadprops;	/* removed properties */
	struct	device_node *parent;
	struct	device_node *child;
	struct	device_node *sibling;
#if defined(CONFIG_OF_KOBJ)
	struct	kobject kobj;
#endif
	unsigned long _flags;
	void	*data;
#if defined(CONFIG_SPARC)
	unsigned int unique_id;
	struct of_irq_controller *irq_trans;
#endif
};
```

```c
struct property {
	char	*name;
	int	  length;
	void	*value;
	struct property *next;
	unsigned long _flags;
	unsigned int unique_id;
	struct bin_attribute attr;
};
```

```c
struct of_changeset {
	struct list_head entries;
};

struct list_head {
	struct list_head *next, *prev;
};
```
# of_find_compatible_node()

```c
struct device_node *of_find_compatible_node(struct device_node *from, const char *type, const char *compat);
```

* ``from``: if ``NULL``, search the entire device tree
* ``type``: node type

If ``from=NULL`` and ``type=NULL``, ``of_find_compatible_node()`` will return the first ``device_node`` it encounters in the device tree while there are still more ``device_node`` with the same ``compat`` in the device tree.

This is the setback of ``of_find_compatible_node()`` when trying to get all nodes with the same ``compat`` but none of them as ``type``, as only one ``device_node`` is returned.

## Find a node device with compatible string ``COMPATIBLE``

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

## Find all nodes with compatible string ``COMPATIBLE``

To find all nodes with the compatible string, [recursive function](https://github.com/TranPhucVinh/C/blob/master/Introduction/Function/README.md#recursive-function) must be used. Check [find_all_nodes_with_comp_str.c](find_all_nodes_with_comp_str.c) for this implementation.

# of_find_node_by_type()

```c
struct device_node *of_find_node_by_type(struct device_node *from, const char *type);
```

Find a node by its ``device_type`` property

# of_find_property()

```c
struct property *of_find_property(const struct device_node *np, const char *name, int *lenp);
```

* ``np``: device node
* ``name``: property name
* ``lenp``: number of bytes of the property value

# of_property_read_u32_array()

Read ``interrupts`` properties from node ``new_dt_node``

```
new_dt_node {
        compatible = "compatible_string";     
        interrupts = <0x0 0x23 0x1 0x0 0x24 0x1>;
};
```

```c
int int_array[6];
struct device_node  *dev_node;
int init_module(void)
{
	dev_node = (struct device_node *)kmalloc(sizeof(struct device_node), GFP_KERNEL);
	dev_node = of_find_compatible_node(NULL, NULL, "compatible_string");
	
	prop = kmalloc(sizeof(struct property*), GFP_KERNEL);

	int ret = of_property_read_u32_array(dev_node, "interrupts", int_array, 6);
	printk("ret %d\n", ret);
	printk("%d %d %d\n", int_array[0], int_array[1], int_array[2]);
}
```

Note that the ``size`` parameter to read must be fixed to the existed size of the property. E.g: As ``interrupts`` array size is ``6`` but use array size ``100`` to read, then there will be error ``EOVERFLOW`` (``75``).

# kbasename()

``kbasename()`` will get the fill name of a node, which is identical to ``dev_node->full_name`` 

```c
dev_node = of_find_compatible_node(NULL, NULL, "simple-bus");
printk("kbasename: %s\n", kbasename(dev_node->full_name));//serial@e0000000
printk("full_name: %s\n", dev_node->full_name);//serial@e0000000
```

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
