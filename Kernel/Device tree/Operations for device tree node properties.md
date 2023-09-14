# General information

For safety purpose, we will change the property of the added overlay device tree so that no built-in device tree properties are effected.

General define for Create, Update and Delete

```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/slab.h> //for kmalloc()

DEFINE_MUTEX(of_mutex);
struct of_changeset *ocs;
```

# [Read device tree node properties](Read%20device%20tree%20node%20operations.md)

Read properties of an overlay node.

Overlay node:

```sh
/dts-v1/;
/plugin/;
/ {
	compatible = "brcm,bcm2835";
	fragment@0 {
        target-path = "/";
		__overlay__ {
			new_dt_node {
				compatible = "compatible_string";
                                device_type = "new_dt_node";
                                string_val_1 = "String value 1";
                                string_val_2 = "String value 2";
                                int_val_1    = <123>;
				int_array	 = <1 2 3 4>;
			};
		};
	};
};
```

## Using platform driver function

With device tree node ``new_dt_node`` is inserted in the device tree, ``probe()`` function in platform driver is called.

``compatible_string`` in ``compatible`` is used by ``struct of_device_id``.

Running ``rmmod`` will call platform driver ``remove()`` function.

Read all properties of ``new_dt_node``: [platform_driver_read_dt_node_properties.c](src/platform_driver_read_dt_node_properties.c)

## Using overlay device tree function

Node functions like [of_find_node_by_type()](API.md#of_find_node_by_type), of_find_compatible_node(),... support reading nodes of device tree.

Property functions like [of_find_property()](API.md#of_find_property), [of_property_read_u32_array()](API.md#of_property_read_u32_array),... will support reading properties values inside a specific node.

# Create operation for device tree

## Add a new node

Add a new node to the device tree by a kernel module requires complicated setup and still can't be implemented. Use the [dtoverlay command](Device%20tree%20overlay.md#create-an-overlay-device-tree-node) or [modify the dtsi file](Add%20a%20new%20node%20to%20device%20tree%20by%20dtsi%20file%20modification.md) to add a new node instead.

## Add a new property to an overlay node

```c
#define COMPATIBLE    	"compatible_string"
#define PROP_VAL     	"new_property"
#define ADD_VAL   		"new_property_string"

int add_node_properties(struct of_changeset *ocs, const char* compatible, const char* property_name, const char *value){
	struct device_node  *dev_node;
	struct property  	*prop;

	dev_node = of_find_compatible_node(NULL, NULL, compatible);
	
	of_changeset_init(ocs);//Init of_changset objects

	prop = kmalloc(sizeof(struct property), GFP_KERNEL);
	prop->name = kstrdup(property_name, GFP_KERNEL);//property name
	prop->value = kstrdup(value, GFP_KERNEL);//property value
	prop->length = strlen(value) + 1;

	of_changeset_add_property(ocs, dev_node, prop);
	
	mutex_lock(&of_mutex);
	int ret = of_changeset_apply(ocs);
	if(ret < 0) {
		printk("Unable to apply changset, ret: %d\n", ret);
		return -1;
	}
	mutex_unlock(&of_mutex);

	return 0; 
}

int init_module(void)
{
	mutex_init(&of_mutex);
    ocs = kmalloc(sizeof(struct of_changeset), GFP_KERNEL);
	add_node_properties(ocs, COMPATIBLE, PROP_VAL, ADD_VAL);	
	return 0;
}
```

# [Update device tree node properties](Update%20device%20tree%20node%20properties.md)

# [Delete operation for device tree](Delete%20operation%20for%20device%20tree.md)
