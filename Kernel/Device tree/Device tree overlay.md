# Create an overlay device tree node

Create an overlay device tree with node name ``new_dt_node`` from file ``dt_overlay_test.dts``:

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

``dtoverlay -a`` will list out all the existed device tree files, ``dt_overlay_test`` file inserted before will have the asterisk sign before its name:

```shell
  draws
* dt_overlay_test
  dwc-otg
  dwc2
  edt-ft5406
```

## Add an overlay node with label

```
/dts-v1/;
/plugin/;
/ {
    compatible = "brcm,bcm2835";
    fragment@0 {
		target-path = "/";
		__overlay__ {
			new_node_label: new_dt_node {
				compatible = "compatible_string";
			};
        };
	};
};
```

After inserting the node, the device tree will be:

```
new_dt_node {
        compatible = "compatible_string";
        phandle = <0x94>;
};
```

Overlay node must not have unit address like this

```
/dts-v1/;
/plugin/;
/ {
    compatible = "brcm,bcm2835";
    fragment@0 {
		target-path = "/";
		__overlay__ {
			new_node_label: new_dt_node@1 {
				compatible = "compatible_string";
			};
        };
	};
};
```

This will give error when compiling with ``dtc`` as there is no ``reg`` value:

```
dt_overlay_test.dts:8.34-10.6: Warning (unit_address_vs_reg): /fragment@0/__overlay__/new_dt_node@1: node has a unit name, but no reg property
```

However, ``reg`` value can't be setup randomly or used the existed one from other node, as this will still give compilation error when compilding with ``dtc``.

# Overlay device tree add value to other target

Add overlay node to another path, like ``reserved-memory``:

```c
/dts-v1/;
/plugin/;
/ {
    compatible = "brcm,bcm2835";
    fragment@0 {
		target-path = "/reserved-memory";
		__overlay__ {
			new_dt_node {
				compatible = "compatible_string";
			};
        };
	};
};
```

After inserting, ``reserved-memory`` will be (node ``new_dt_node`` is added):

```c
reserved-memory {
		ranges;
		#address-cells = <0x01>;
		#size-cells = <0x01>;
		phandle = <0x36>;

		new_dt_node {
				compatible = "compatible_string";
				phandle = <0x94>;
		};

		linux,cma {
				reusable;
				compatible = "shared-dma-pool";
				size = <0x10000000>;
				phandle = <0x37>;
				linux,cma-default;
		};
};
```

Notice that, node ``new_dt_node`` now has ``phandle = <0x94>``.

Node like ``cpus`` can be add normally by an overlay node with ``target-path = "/cpus"``. After inserting, it will have ``phandle = <0x96>``.

## Add multiple fragments in overlay device tree

**Overlay dts with path "/"**

```sh
/dts-v1/;
/plugin/;
/ {
    compatible = "brcm,bcm2835";
    fragment@0 {
		target-path = "/";
		__overlay__ {
			new_dt_node_1 {
				compatible = "compatible_string";
			};
        };
	};

	fragment@1 {
		target-path = "/";
		__overlay__ {
			new_dt_node_2 {
				compatible = "compatible_string";
			};
        };
	};
};
```

**Overlay dts with 2 different paths**

```c
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

	fragment@1 {
		target-path = "/reserved-memory";
		__overlay__ {
			new_dt_node {
				compatible = "compatible_string";
			};
        };
	};
};
```

Result in device tree file

```c
new_dt_node {
        compatible = "compatible_string";
};

reserved-memory {
        ranges;
        #address-cells = <0x01>;
        #size-cells = <0x01>;
        phandle = <0x36>;

        new_dt_node {
                        compatible = "compatible_string";
        };

        linux,cma {
                        reusable;
                        compatible = "shared-dma-pool";
                        size = <0x10000000>;
                        phandle = <0x37>;
                        linux,cma-default;
        };
};
```

# Read all device tree nodes, include device tree overlay nodes

Check [dtoverlay -a]() command implementation in [Create an overlay device tree node]()

# Remove the inserted device tree overlay nodes

To remove all the inserted device tree overlay nodes from file ``dt_overlay_test.dts``: ``sudo dtoverlay -r dt_overlay_test``.

