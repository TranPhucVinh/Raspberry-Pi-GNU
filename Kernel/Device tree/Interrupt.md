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
