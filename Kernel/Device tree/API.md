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

Implementations:
* [Find a node device with compatible string ``COMPATIBLE``](Read%20device%20tree%20node%20operations.md#of_find_compatible_node-to-find-a-node-device-with-compatible-string-compatible)
* [Find all nodes with compatible string ``COMPATIBLE`` by using recursive](Read%20device%20tree%20node%20operations.md#of_find_compatible_node-to-find-all-nodes-with-compatible-string-compatible-by-using-recursive)

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
```c
int of_property_read_u32_array(const struct device_node *np, const char *propname, u32 *out_values, size_t sz);
```
* ``size_t sz``: number of array elements to read

Implementation: [Read ``interrupts`` properties from node ``new_dt_node``](Read%20device%20tree%20node%20operations.md#of_property_read_u32_array)

# of_node_get()

```c
struct device_node *of_node_get(struct device_node *node)
```
Increment [refcount](https://github.com/TranPhucVinh/C/tree/master/Kernel/Character%20device/Character%20device%20operations#kobject-and-refcount) of a node

# of_node_put()

```c
void of_node_put(struct device_node *node)
```

Decrement [refcount](https://github.com/TranPhucVinh/C/tree/master/Kernel/Character%20device/Character%20device%20operations#kobject-and-refcount) of a node

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

Implementation: [Find a node with compatile string ``simple-bus`` and print out the ``name`` and ``full_name`` of all of its child nodes](Read%20device%20tree%20node%20operations.md#for_each_child_of_node)