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

# GPIO control

* [blink_led.c](blink_led.c): Blink LED by GPIO kernel API and kernel timer setup
* [control_led_with_button.c](control_led_with_button.c): Use button to control the LED. If button is pressed, turn on LED, if not pressed, turn off LED.

Control GPIO with ``linux/io``:

* [blink_led_with_ioremap.c](blink_led_with_ioremap.c): Blink LED with ``ioremap()``
* [blink_led_with_iowrite32.c](blink_led_with_iowrite32.c): Blink LED with ``ioremap()`` and ``iowrite32()``

# Interrupt

## Toggle LED by interrupt

Toggle LED status by pressing button with interrupt. If pressed button for the first time, LED turn on, press one more time, LED turn off.

**Program**: [toggle_led_by_gpio_interrupt.c](toggle_led_by_gpio_interrupt.c)

After inserting the module, the IRQ number registered by the GPIO and interrupt name can be viewed in proc/interrupts

```
           CPU0       CPU1       CPU2       CPU3   
199:        446          0          0          0  pinctrl-bcm2835   3 Edge      Device name
```
