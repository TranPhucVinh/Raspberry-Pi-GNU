# Fundamental concepts

Unpack the device tree and view on terminal: ``dtc -I fs /sys/firmware/devicetree/base``

Unpack the device tree to a file: ``(dtc -I fs /sys/firmware/devicetree/base) > device_tree.md``

Create an overlay device tree with node name ``new_dt_node``:

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
			};
        };
	};
};
```

Compile the overlay device tree in Raspbian:

```sh
dtc -@ -I dts -O dtb -o dt_overlay_test.dtbo dt_overlay_test.dts
```

Insert the overlay device tree to the existed device tree (must run with ``sudo``):

```sh
sudo dtoverlay dt_overlay_test.dtbo
```

Before inserting the overlay device tree, node ``new_dt_node`` is not existed:

```c
/ {
        compatible = "raspberrypi,3-model-b\0brcm,bcm2837";
        serial-number = "000000004bfc6d54";
        model = "Raspberry Pi 3 Model B Rev 1.2";
        memreserve = <0x3b400000 0x4c00000>;
        interrupt-parent = <0x01>;
        #address-cells = <0x01>;
        #size-cells = <0x01>;

        reserved-memory {
                ranges;
                #address-cells = <0x01>;
                #size-cells = <0x01>;
                phandle = <0x36>;
		...
```

After inserting the overlay device tree above, node ``new_dt_node`` will appear:

```c
/ {
        compatible = "raspberrypi,3-model-b\0brcm,bcm2837";
        serial-number = "000000004bfc6d54";
        model = "Raspberry Pi 3 Model B Rev 1.2";
        memreserve = <0x3b400000 0x4c00000>;
        interrupt-parent = <0x01>;
        #address-cells = <0x01>;
        #size-cells = <0x01>;

        new_dt_node {
                compatible = "compatible_string";
        };

        reserved-memory {
                ranges;
                #address-cells = <0x01>;
                #size-cells = <0x01>;
                phandle = <0x36>;

                linux,cma {
		...
```

## Parse device node properties

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

# API

Functions to read device tree node properties like ``device_property_present()`` are only available for platform devices, running th function inside character device will result in error.
