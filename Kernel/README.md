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

## GPIO driver

Kernel module as driver to control GPIO:
* [blink_led.c](blink_led.c): Blink LED by GPIO kernel API and kernel timer setup
* [control_led_with_button.c](control_led_with_button.c): Use button to control the LED. If button is pressed, turn on LED, if not pressed, turn off LED.

Control GPIO with ``linux/io``:

* [blink_led_with_ioremap.c](blink_led_with_ioremap.c): Blink LED with ``ioremap()``
* [blink_led_with_iowrite32.c](blink_led_with_iowrite32.c): Blink LED with ``ioremap()`` and ``iowrite32()``
* [control_led_with_ioread32.c](control_led_with_ioread32.c): Use button to control the LED. If button is pressed, turn on LED, if not pressed, turn off LED. Using ``ioread32()`` and ``iowrite32()``.

## Interrupt with GPIO

## API

In Raspbian, every GPIO will have a unique IRQ number. ``gpio_to_irq()`` will return that unique number of each GPIO.

## Example 1: Blink LED and count how many times IRQ is triggered

**Features**:

* Map a GPIO to an IRQ then blink that GPIO with a thread
* Count how many times that IRQ is triggered in the IRQ handler (as it is triggered everytime it blinks)

**Program**: [gpio_interrupt.c](gpio_interrupt.c)

**Program**: [toggle_led_by_gpio_interrupt.c](toggle_led_by_gpio_interrupt.c)

After inserting the module, the IRQ number registered by the GPIO and interrupt name can be viewed in ``proc/interrupts``

```
           CPU0       CPU1       CPU2       CPU3   
199:        126          0          0          0  pinctrl-bcm2835   2 Edge      GPIO_IRQ
```

## Example 2: Toggle LED by GPIO interrupt

**Features**:

* Toggle LED status by pressing button with interrupt. If pressed button for the first time, LED turn on, press one more time, LED turn off
* Count how many times the GPIO interrupt is triggered.

**Trigger the registered GPIO interrupt by a userspace program**: [direct_register_access_control_led_with_button.c](https://github.com/TranPhucVinh/Raspberry-Pi-C/blob/main/Physical%20layer/GPIO/direct_register_access_control_led_with_button.c), a userspace program can trigger the IRQ number registered by this kernel module. The IRQ handler message in this kernel will then print out how many times the GPIO interrupt is triggered every time the button is pressed when running this user space program.

## Disable GPIO interrupt 

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