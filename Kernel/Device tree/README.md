# Fundamental concepts

In Raspbian or Unix single board computers in general, device tree are opened to read in ``/sys/firmware/devicetree/``.

To unpack the device tree and view on terminal: ``dtc -I fs /sys/firmware/devicetree/base``

Unpack the device tree to a file: ``(dtc -I fs /sys/firmware/devicetree/base) > device_tree.md``

## Device tree overlay

Check [device tree overlay document](Device%20tree%20overlay.md).

## Parse device node properties

Check [parse device node properties document](Parse%20device%20tree%20node%20properties.md).

## Interrupt

Check [Interrupt document](Interrupt.md).

# API

## Platform driver functions

Functions to read device tree node properties like ``device_property_present()`` are only available for platform devices, running those function inside character device will result in error.

### platform_get_irq()

```c
int platform_get_irq(struct platform_device *dev, unsigned int irq_index)
```

* ``irq_index``: Index number of the IRQs inside ``interrupts`` property of the device tree node

E.g

```c
int irq = platform_get_irq(pdev, 0); 
```

This function will return the ``irq`` number; this number is usable by ``devm_request_irq()`` (``irq`` is then visible in ``/proc/interrupts``). The second argument, ``0``, says that we need the first interrupt specified in the device node. If there is more than one interrupt, we can change this index according to the interrupt we need.

## Device tree node parsing API

### struct

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

### of_find_node_by_type()

```c
struct device_node *of_find_node_by_type(struct device_node *from, const char *type);
```

Find a node by its ``device_type`` property

* ``from``: If ``NULL``, search the entire device tree
* ``type``: ``device_type`` propery value

**Example**: Find a node device with ``device_type`` is ``serial`` in a kernel module

```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/slab.h> //for kmalloc()

MODULE_LICENSE("GPL");

struct device_node *dev_node;

int init_module(void)
{
	dev_node = (struct device_node *)kmalloc(sizeof(struct device_node), GFP_KERNEL);
	dev_node = of_find_node_by_type(NULL, "memory");
	printk("name: %s\n", dev_node->name);//memory
	printk("full_name: %s\n", dev_node->full_name);//memory@0
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "clean up module\n");
}
```

### APIs to update device tree node properties

```c
void of_changeset_init(struct of_changeset *ocs);

int of_changeset_attach_node(struct of_changeset *ocs, struct device_node *np);

int of_overlay_fdt_apply(const void *overlay_fdt, u32 overlay_fdt_size, int *ovcs_id);
```

### of_property_read_u32_array()

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
