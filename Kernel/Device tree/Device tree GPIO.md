Take this overlay device tree for analysis in API document and in the example to control GPIO:

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
				led-gpios = <&gpio 2 0>,
				<&gpio 3 0>,
				<&gpio 4 0>;
			};
        };
	};
};
```
After inserting the new device tree node will be:

```
new_dt_node {
                led-gpios = <0x07 0x02 0x00 0x07 0x03 0x00 0x07 0x04 0x00>;
                compatible = "compatible_string";
};
```

# API

### GPIOs mapping

GPIOs mappings are defined in the consumer device's node, in a property named ``<function>-gpios``, where ``<function>`` is the function the driver will request through ``gpiod_get()``.

E.g: In [device_tree_gpio_control.c](device_tree_gpio_control.c) source code:

```c
struct gpio_desc *red, *green, *blue;

red = gpiod_get_index(dev, "led", 0, GPIOD_OUT_HIGH);
green = gpiod_get_index(dev, "led", 1, GPIOD_OUT_HIGH);
blue = gpiod_get_index(dev, "led", 2, GPIOD_OUT_HIGH);
```

```c
#include <linux/gpio/consumer.h>

struct gpio_desc *gpiod_get(struct device *dev, const char *con_id, enum gpiod_flags flags)

struct gpio_desc *gpiod_get_index(struct device *dev, const char *con_id, unsigned int idx, enum gpiod_flags flags)
```

The ``flags`` parameter is used to optionally specify a direction and initial value for the GPIO, with values:

* ``GPIOD_IN`` to initialize the GPIO as input.
* ``GPIOD_OUT_LOW`` to initialize the GPIO as output with a value of 0.
* ``GPIOD_OUT_HIGH`` to initialize the GPIO as output with a value of 1.

Third parameter of every gpio member (in the overlay device tree) can be defined with ``GPIO_ACTIVE_HIGH`` and ``GPIO_ACTIVE_LOW`` like this:

```txt
led-gpios = <&gpio 2 GPIO_ACTIVE_HIGH>, <&gpio 3 GPIO_ACTIVE_HIGH>, <&gpio 4 GPIO_ACTIVE_HIGH>;
```

Where:

* ``GPIO_ACTIVE_HIGH``: Write logic 1 to set 5V
* ``GPIO_ACTIVE_LOW``: Write logic 1 to set 0V

In Raspbian Kernel header, those macros are mapped to:

```c
GPIO_ACTIVE_HIGH  = 0
GPIO_ACTIVE_LOW   = 1
```

### Setting Direction

A GPIO control kernel driver must set GPIO direction before any R/W operation. If no direction-setting flags have been given to ``gpiod_get*()``, this is done by invoking one of the ``gpiod_direction_*()`` functions:

```c
int gpiod_direction_input(struct gpio_desc *desc);
int gpiod_direction_output(struct gpio_desc *desc, int value);
```

A driver can also query the current direction of a GPIO:

```c
int gpiod_get_direction(const struct gpio_desc *desc)
```

# Implementation: Control GPIO with GPIO node from device tree overlay

**Feature**: Blink 3 LED connected to 3 GPIOs. 3 those GPIOs are defined in the overlay device tree

Program [device_tree_gpio_control.c](device_tree_gpio_control.c)
