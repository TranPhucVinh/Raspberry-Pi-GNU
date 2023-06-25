# Properties

* ``interrupt-parent`` is a phandle that points to the interrupt controller for the current node.
* ``interrupts``: A general value depends on type of the ``interrupt-parent``. E.g with with ``interrupt-parent`` is ``gpio``, then the ``interrupts`` has the GPIO number as one of its properties.
* ``#interrupt-cells`` indicates the number of cells in the interrupts property for the interrupts managed by the
selected interrupt controller.
* [interrupt-controller](#interrupt-controller)

## interrupt-controller

``interrupt-controller`` must be set with valid properties for parsing. If adding a new ``interrupt-controller`` as an overlay node like this:

```c
/dts-v1/;
/plugin/;
/ {
    compatible = "brcm,bcm2835";
    fragment@0 {
		target-path = "/";
		__overlay__ {
			new_interrupt_controller_node {
					compatible = "new_interrupt_controller_node";
					#interrupt-cells = <0x01>;
					interrupt-controller;
			};
        };
	};
};
```

Then add an overlay node to point to that interrupt-controller:

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
                interrupt-parent = <&new_interrupt_controller_node>;
                interrupts   = <199>;
			};
		};
	};
};
```

There will be error when running ``dtoverlay``:

```sh
pi@raspberrypi:~/Raspbian_Kernel_Module $ sudo dtoverlay new_node.dtbo
* Failed to apply overlay '2_new_node' (kernel)
```

Error in ``dmesg``:

```
[ 3395.762453] OF: resolver: node label 'new_interrupt_controller_node' not found in live devicetree symbols table
[ 3395.762484] OF: resolver: overlay phandle fixup failed: -22
```

# Implement GPIO interrupt for platform driver

GPIO nodes in device tree with interrupts properties:

```c
gpio@7e200000 {
	compatible = "brcm,bcm2835-gpio";
	//Other values here
	#interrupt-cells = <0x02>;
	interrupts = <0x02 0x11 0x02 0x12>;
	phandle = <0x07>;
	reg = <0x7e200000 0xb4>;
	#gpio-cells = <0x02>;
	pinctrl-names = "default";
	interrupt-controller;

	uart0_gpio14 {
		brcm,pins = <0x0e 0x0f>;
		phandle = <0x4e>;
		brcm,function = <0x04>;
	};
	//Other values here
}
```

The properties of GPIO states has ``interrupt-controller`` which infers that it is also an interrupt controller.

Add GPIO interrupt to [an overlay device tree](Device%20tree%20overlay.md)

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

Or [modify file bcm2710-rpi-3-b.dts with a new dtsi file, like my_dtsi.dtsi, to add a new node](Add%20a20new20node20to20device20tree20by20dtsi20file%20modification.md).

As ``#interrupt-cells`` is ``0x02``, so every GPIO interrupt need 2 values. In this example:

* ``3``: GPIO pin number of Raspberry that wished to be converted to IRQ number
* ``1``: IRQ flag for ``IRQF_TRIGGER_RISING``

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

1. Platform driver gets IRQ number from GPIO 3 that is registered in device tree overlay. As being GPIO interrupt, i.e **external interrupt**, **GPIO 3 must be register as input**.
2. Register to that IRQ and trigger those IRQ from userspace, IRQ function handler will print out how many times that IRQ is triggered
3. **Trigger interrupt**: As GPIO 3 is input, interrupt must be triggered by hardware, i.e using button to turn on/off GPIO 3

**Program**: [platform_driver_interrupt.c](platform_driver_interrupt.c)

**Notice**: If register GPIO 3 as input, like setting with ``devmem`` like this:

```sh
sudo busybox devmem 0x3f200000 w 0x200 	#Set output for GPIO 3
```
Then the IRQ is registered successfully then perform the IRQ handler normally (by devmem write GPIO HIGH/LOW) but there are memory issue when parsing the device tree to get the IRQ number. Check [platform_driver_interrupt_dmesg_log.txt](platform_driver_interrupt_dmesg_log.txt) for that memory issue

# Get interrupt number by name by platform_get_irq_byname()

```
/dts-v1/;
/plugin/;
/ {
	compatible = "brcm,bcm2837";
	fragment@0 {
        target-path = "/";
		__overlay__ {
			new_dt_node {
				compatible = "compatible_string";
                interrupt-parent = <&gpio>;
                interrupts   = <3 1>, <2 1>;
				interrupt-names = "gpio_3_irq", "gpio_2_irq";
			};
		};
	};
};
```

Program [get_interrupt_by_name.c](get_interrupt_by_name.c) gets interrupt number by name by using ``platform_get_irq_byname()`` function for GPIO 2 and GPIO 3
