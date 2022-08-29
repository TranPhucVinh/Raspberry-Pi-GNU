# Properties

* ``interrupt-parent`` is a phandle that points to the interrupt controller for the current node.
* ``interrupts``: A general value depends on type of the ``interrupt-parent``. E.g with with ``interrupt-parent`` is ``gpio``, then the ``interrupts`` has the GPIO number as one of its properties.

* ``#interrupt-cells`` indicates the number of cells in the interrupts property for the interrupts managed by the
selected interrupt controller.

## Interrupt implementation for platform driver

## GPIO interrupt

GPIO nodes in device tree with interrups properties:

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

```

The properties of GPIO states has ``interrupt-controller`` which infers that it is also an interrupt controller.

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

### Get interrupt number by name

Program [get_interrupt_by_name.c](get_interrupt_by_name.c)
