**General information**: For safety purpose, we will change the property of the added overlay device tree.

## Properties

**Array** in device tree are inside ``<>``, each member is seperated by space. ``,`` is not allowed to used to seperate each member.

E.g: An int array properties inside a devie tree node:

```c
int_array	 = <1 2 3 4>;
```

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

Kernel module: [update_property_for_overlay_device_tree.c](update_property_for_overlay_device_tree.c)

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