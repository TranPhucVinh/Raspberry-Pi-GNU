**General information**: For safety purpose, we will change the property of the added overlay device tree.

## Parse properties of an overlay node

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

With device tree node ``new_dt_node`` is inserted in the device tree, ``probe()`` function in platform driver is called.

Only a platform driver can be used to parse the properties of a device tree node.

``compatible_string`` in ``compatible`` is used by ``struct of_device_id``.

Running ``rmmod`` will call platform driver ``remove()`` function.

Parse all properties of ``new_dt_node``: [parse_device_tree_node_properties.c](parse_device_tree_node_properties.c)

## Change property of an added overlay device tree node by a kernel module

Linux support kernel API for CRUD operations to device tree nodes in a single board computer Unix OS.

### Change string properties

**Change string properties by single operations**

Features: Change string properties of any node in the device tree by single operations. This example is intended for comprehensive and easy to understand the device tree changeset overlay operations.

Program: [update_property_for_node_by_single_functions.c](update_property_for_node_by_single_functions.c)

Before change:

```sh
new_dt_node {
        compatible = "compatible_string";
        device_type = "new_dt_node";
        string_val_1 = "String value 1";
        string_val_2 = "String value 2";
        int_val_1    = <123>;
        int_array	 = <1 2 3 4>;
};
```

Change ``compatible`` property to ``new_compatible_string``.

```sh
new_dt_node {
        compatible = "new_compatible_string";
        device_type = "new_dt_node";
        string_val_1 = "String value 1";
        string_val_2 = "String value 2";
        int_val_1    = <123>;
        int_array	 = <1 2 3 4>;
};
```

With the kernel module program above, as using the overlay device tree node with ``target-path = "/"``, its parent node will be ``NULL``.

**Change string properties by seperated operations**

Features: Seperated the device tree changeset overlay operations by function ``update_node_properties()`` to update the node properties and revert to the original device tree by function ``revert_to_origrin_device_tree()``. This program is intended for functions inheritance for other kernel modules.

Program: [update_property_for_node_by_seperated_functions.c](update_property_for_node_by_seperated_functions.c)

### Change array properties

**Feature**: Change ``int_array`` properties in the added overlay node

**Program**:

```c
//Other operations like update_property_for_overlay_device_tree.c
#define SIZE	4

int int_array[SIZE] = {0x05000000, 0x06000000, 0x07000000, 0x08000000};

int init_module(void)
{
        //Other operations like update_property_for_overlay_device_tree.c
	prop = kmalloc(sizeof(struct property*), GFP_KERNEL);
	prop->name = kstrdup("int_array", GFP_KERNEL);//property name
	prop->value = int_array;//property value
	prop->length = sizeof(int)*SIZE;

	//Update the "compatible" property
	of_changeset_update_property(ocs, dev_node, prop);

        //Other operations like update_property_for_overlay_device_tree.c
}
```

**Notes**: If changing array like this

```c
#define SIZE	4

int int_array[SIZE] = {5, 6, 7, 8};
```

The result will be:

```c
int_array =  <0x5000000 0x6000000 0x7000000 0x8000000>;
```

### Change int property

Change a single int value of a node property, like ``int_val_1`` in the overlay node above:

Change ``int_val_1`` by function ``update_node_properties()``:

```c
#define NODE_TYPE    "new_dt_node"
#define PROP_VAL     "int_val_1"

int change_value = 456;

int init_module(void)
{
	mutex_init(&of_mutex);
    ocs = kmalloc(sizeof(struct of_changeset), GFP_KERNEL);
	update_node_properties(ocs, NODE_TYPE, PROP_VAL, &change_value);
	return 0;
}

int update_node_properties(struct of_changeset *ocs, const char* node_type, const char* property_name, int *update_value){
	struct device_node  *dev_node;
	struct property  	*prop;

	dev_node = of_find_node_by_type(NULL, node_type);
	
	//Init of_changset objects
	of_changeset_init(ocs);

	prop = kmalloc(sizeof(struct property), GFP_KERNEL);
	prop->name = kstrdup(property_name, GFP_KERNEL);//property name
	prop->value = update_value;//property value
	prop->length = sizeof(int);

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
```

Note: If using this as ``change_value`` is the local variable of ``init_module()``, then ``int_val_1`` will be ``0x00`` (error value). That happen as local variable ``change_value`` will lost its value when getting outside the ``init_module()`` function so it gives no effect in the device tree when viewing it by ``dtc`` command.