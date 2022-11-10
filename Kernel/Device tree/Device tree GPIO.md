# Control GPIO with GPIO node from device tree overlay

Feature: Blink 3 LED connected to 3 GPIOs. 3 those GPIOs are defined in the overlay device tree

Overlay device tree

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

Program [device_tree_gpio_control.c](device_tree_gpio_control.c)