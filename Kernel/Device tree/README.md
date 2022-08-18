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

Check [parse device node properties document](Parse%20device%20tree%20node%20properties.md).

## Interrupt

``interrupt-parent`` is a phandle that points to the interrupt controller for the current node.

``interrupts``: A general value depends on type of the ``interrupt-parent``. E.g with with ``interrupt-parent`` is ``gpio``, then the ``interrupts`` is the GPIO number.

Add GPIO interrupt to an overlay device tree

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
                interrupt-parent = <&gpio>;
                interrupts   = <3 1>;
			};
		};
	};
};
```

``3``: GPIO pin number of Raspberry that wished to be converted to IRQ number

``1``: IRQ flag for ``IRQF_TRIGGER_RISING``

Note that the ``interrupt-parent`` must have value ``<&gpio>``, as it is the GPIO IRQ. Without ``interrupt-parent``, there will be warning/error:

```c
dt_overlay_test.dts:8.16-14.6: Warning (interrupts_property): /fragment@0/__overlay__/new_dt_node: Missing interrupt-parent
```

If using invalid value for ``interrupt-parent`` like ``interrupt-parent = <0x1b>``, there will be warning/error:

```
dt_overlay_test.dts:13.17-43: Warning (interrupts_property): /fragment@0/__overlay__/new_dt_node:interrupt-parent: Bad phandle
```

IRQ flag values are defined in ``linux/interrupt.h``

```c
#define IRQF_TRIGGER_NONE	0x00000000
#define IRQF_TRIGGER_RISING	0x00000001
#define IRQF_TRIGGER_FALLING	0x00000002
#define IRQF_TRIGGER_HIGH	0x00000004
#define IRQF_TRIGGER_LOW	0x00000008
```

**Feature**

1. Platform driver gets IRQ number from GPIO 3 that is registered in device tree overlay
2. Register to that IRQ and trigger those IRQ from userspace, IRQ function handler will print out how many times that IRQ is triggered

**Program**: [platform_driver_interrupt.c](platform_driver_interrupt.c)

**Notice**

Before ``isnmod`` the platform_driver_interrupt kernel module, register that mapped to GPIO 3 must be allocated and set to a mode like input or output mode. This is mapped to the operation ``gpio_direction_output(GPIO, 1)``. Without this setup operation, there will be error:

```
[25064.080814] gpio gpiochip0: (pinctrl-bcm2835): gpiochip_lock_as_irq: cannot get GPIO direction
[25064.080831] gpio gpiochip0: (pinctrl-bcm2835): unable to lock HW IRQ 3 for IRQ
[25064.080844] genirq: Failed to request resources for GPIO_3_IRQ (irq 200) on irqchip pinctrl-bcm2835
```

This setup operation can be done by ``busybox devmem``. So run this ``devmem`` command:

```sh
sudo busybox devmem 0x3f200000 w 0x200 	#Set output for GPIO 3
```

# API

### Platform driver functions

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
