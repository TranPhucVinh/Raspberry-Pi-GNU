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

Kernel module: [update_property_for_overlay_device_tree.c](update_property_for_overlay_device_tree.c)

Before change:

```sh
new_dt_node {
        compatible = "compatible_string";
        device_type = "new_dt_node";
        interrupt-parent = <0x07>;
        interrupts = <0x03 0x01>;
};
```

Change ``compatible`` property to ``new_compatible_string``.

```sh
new_dt_node {
        compatible = "compatible_string";
        device_type = "new_compatible_string";
        interrupt-parent = <0x07>;
        interrupts = <0x03 0x01>;
};
```

With the kernel module program above, as using the overlay device tree node with ``target-path = "/"``, its parent node will be ``NULL``.
