# Loadable kernel module

By default, modules stored in ``/lib/modules`` has no ``Makefile`` to build.

For example ``uname -r`` has ``5.10.17-v7+``

``/lib/modules/5.10.17-v7+`` has:

```
kernel                     modules.builtin.bin      modules.order
modules.alias              modules.builtin.modinfo  modules.softdep
modules.alias.bin          modules.dep              modules.symbols
modules.builtin            modules.dep.bin          modules.symbols.bin
modules.builtin.alias.bin  modules.devname
```

(no ``build`` folder)

To build kernel module, user has to install Kernel header for Raspbian: ``sudo apt-get install raspberrypi-kernel-headers``

After installing, ``/lib/modules/5.10.17-v7+`` will have: (has ``build`` folder)

```
build   modules.alias      modules.builtin            modules.builtin.bin      modules.dep      modules.devname  modules.softdep  modules.symbols.bin
kernel  modules.alias.bin  modules.builtin.alias.bin  modules.builtin.modinfo  modules.dep.bin  modules.order    modules.symbols
```

``Makefile`` to build the kernel module:

```Makefile
obj-m += raspbian_kernel_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```    

For kernel module insert and remove, check ``README.md`` in ``C/Kernel/`` as their implementations are identical.

In 12th February 2022, with Raspbian installed from ``2021-10-30-raspios-bullseye-armhf.img``, the module will be ``5.10.63-v7+``. After installing the kernel header for Raspbian, there won't be ``build`` folder inside ``/lib/modules/5.10.17-v7+``.

**Examples**

* ``raspbian_kernel_module.c``: The very first kernel module built on Raspbian

# GPIO driver

Kernel module as driver to control GPIO:
* [blink_led.c](blink_led.c): Blink LED by GPIO kernel API and kernel timer setup
* [control_led_with_button.c](control_led_with_button.c): Use button to control the LED. If button is pressed, turn on LED, if not pressed, turn off LED.

Control GPIO with ``linux/io``:

* [blink_led_with_ioremap.c](blink_led_with_ioremap.c): Blink LED with ``ioremap()``
* [blink_led_with_iowrite32.c](blink_led_with_iowrite32.c): Blink LED with ``ioremap()`` and ``iowrite32()``
* [control_led_with_ioread32.c](control_led_with_ioread32.c): Use button to control the LED. If button is pressed, turn on LED, if not pressed, turn off LED. Using ``ioread32()`` and ``iowrite32()``.

## Interrupt with GPIO

### API

In Raspbian, every GPIO will have a unique IRQ number. ``gpio_to_irq()`` will return that unique number of each GPIO.

### Example 1: Blink LED and count how many times IRQ is triggered

**Features**:

* Map a GPIO to an IRQ then blink that GPIO with a thread
* Count how many times that IRQ is triggered in the IRQ handler (as it is triggered everytime it blinks)

**Program**: [gpio_interrupt.c](gpio_interrupt.c)

After inserting the module, the IRQ number registered by the GPIO and interrupt name can be viewed in ``proc/interrupts``

```
           CPU0       CPU1       CPU2       CPU3   
199:        126          0          0          0  pinctrl-bcm2835   2 Edge      GPIO_IRQ
```

### Example 2: Toggle LED by GPIO interrupt

**Features**:

* Toggle LED status by pressing button with interrupt. If pressed button for the first time, LED turn on, press one more time, LED turn off
* Count how many times the GPIO interrupt is triggered.

**Program**: [toggle_led_by_gpio_interrupt.c](toggle_led_by_gpio_interrupt.c)

**Trigger the registered GPIO interrupt by a userspace program**: [direct_register_access_control_led_with_button.c](https://github.com/TranPhucVinh/Raspberry-Pi-C/blob/main/Physical%20layer/GPIO/direct_register_access_control_led_with_button.c), a userspace program can trigger the IRQ number registered by this kernel module. The IRQ handler message in this kernel will then print out how many times the GPIO interrupt is triggered every time the button is pressed when running this user space program.

### Disable GPIO interrupt 

This kernel module will disable interrupt ``199`` which is registred by the program/kernel module ``toggle_led_by_gpio_interrupt.c`` above. Note that interrupt ``199`` is not needed to be shared (by ``IRQF_SHARED`` flag) to be disable by other kernel module.

```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>

#define IRQ_NUM    199

int init_module(void)
{
	printk(KERN_INFO "Module to disable interrupt\n");
	disable_irq(IRQ_NUM);

	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "clean up module disable interrupt\n");
}
```

After inserting this module, the interrupt name Device name still existed in ``199`` but the interrupt function no longer work.

# I2C driver

Build a I2C driver to control PCF8574 module, turn on a specific GPIO when ``insmod`` successfully: [i2c_driver_pcf8574.c](i2c_driver_pcf8574.c)

After ``insmod`` the driver, device ``/dev/PCF8574_DRIVER`` will be available for userspace communication.

## Control GPIO of PCF8574 module through the device from user space

In driver code, change ``dev_write()`` function to:

```c
#define WRITE_SIZE	1
char write_value[WRITE_SIZE];
ssize_t dev_write(struct file*filep, const char __user *buf, size_t len, loff_t *offset)
{
	memset(write_value, 0, sizeof(write_value));
	copy_from_user(write_value, buf, len);

	//Then turn on a specifc GPIO when insmod successfully
	i2c_master_send(pcf8574_i2c_client, write_value, WRITE_SIZE);
	return sizeof(write_value);
}
```

Userspace program to control GPIO by ``write()`` system call:

```c
#define DEV_NAME    "/dev/PCF8574_DRIVER"
#define WRITE_SIZE	1
#define PIN         4

char write_string[WRITE_SIZE];

int fd;

int main(){
    fd = open(DEV_NAME, O_RDWR);
    if(fd < 0) {
        printf("Fail to open %s\n",DEV_NAME);
        return 1;
    }
    else {
        write_string[0] = 1 << PIN;
        write(fd, write_string, sizeof(write_string));
    }
    return 0;
}
```

# Device tree

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

Insert the overlay device tree to the existed device tree: ``dtoverlay dt_overlay_test.dtbo``

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

## API

Functions to read device tree node properties like ``device_property_present()`` are only available for platform devices, running th function inside character device will result in error.
