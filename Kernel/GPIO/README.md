# GPIO control

Kernel module as driver to control GPIO:
* [blink_led.c](blink_led.c): Blink LED by GPIO kernel API and kernel timer setup
* [control_led_with_button.c](control_led_with_button.c): Use button to control the LED. If button is pressed, turn on LED, if not pressed, turn off LED.

Control GPIO with ``linux/io``:

* [blink_led_with_ioremap.c](blink_led_with_ioremap.c): Blink LED with ``ioremap()``
* [blink_led_with_devm_ioremap.c](blink_led_with_ioremap.c): Blink LED with ``devm_ioremap()`` (for character device)
* [blink_led_with_iowrite32.c](blink_led_with_iowrite32.c): Blink LED with ``ioremap()`` and ``iowrite32()``
* [control_led_with_ioread32.c](control_led_with_ioread32.c): Use button to control the LED. If button is pressed, turn on LED, if not pressed, turn off LED. Using ``ioread32()`` and ``iowrite32()``.

## API

### gpio_* functions

In the latest Linux kernel versions, GPIO allocation before used are no longer require. GPIO output value can be set directly

```c
int gpio_direction_output(unsigned int gpio, int value);
void gpio_set_value(unsigned int gpio, int value);
```

GPIO input value can be get:

```c
int gpio_get_value(unsigned int gpio);
```

### IO mapping functions

When access passes though page tables, the kernel must first arrange for the physical address to be visible from your driver, and this usually means that you must call ioremap before doing any I/O. 

```c
void *ioremap(unsigned long phys_addr, unsigned long size);
```

The ``ioremap`` function takes two parameters:

* start of the memory region
* size of the memory region

``ioremap()`` function is used to map the physical address of an I/O device to the kernel virtual address. Kernel creates a page table i.e mapping of virtual address to the physical address requested.

A successful call to ``ioremap()`` returns a kernel virtual address corresponding to start of the requested physical address range

```c
void iounmap(void * addr);
```

Corresponded functions of ``ioremap()`` and ``iounmap()`` for character devices:

```c
void __iomem *devm_ioremap(struct device *dev, resource_size_t offset, resource_size_t size);
void devm_iounmap(struct device *dev, void __iomem *addr);
```

To read from I/O memory, use one of the following:

```c
unsigned int ioread8(void *addr);
unsigned int ioread16(void *addr);
unsigned int ioread32(void *addr);
```

There is a similar set of functions for writing to I/O memory:

```c
void iowrite8(u8 value, void *addr);
void iowrite16(u16 value, void *addr);
void iowrite32(u32 value, void *addr);
```

# Interrupt with GPIO

By default, GPIO 2 and 14 of Raspberry Pi 3B+ are already registered with 2 register. ``cat /proc/interrupts`` returns:

```
           CPU0       CPU1       CPU2       CPU3  
200:     355860          0          0          0  pinctrl-bcm2835  14 Edge      gpiolib
201:          1          0          0          0  pinctrl-bcm2835   2 Edge    
```

GPIO 2 can be registered to interrupt normally with ``request_irq()`` while **GPIO 14 can't be registered** to any other interrupt as it belongs to ``gpiolib``.

## Fundamental concepts

In Raspbian, every GPIO will have a unique IRQ number. ``gpio_to_irq()`` will return that unique number of each GPIO.

Flow to register the GPIO for IRQ:

1. GPIO to be interrupt **must be input**:

```C
if(gpio_direction_input(GPIO)){
  printk("Unable to set GPIO %d to input\n", GPIO);
}
```

If using ``gpio_request(GPIO, LABEL)`` only and doesn't call I/O mode setup, ``request_irq()`` in step 3 will fail.

2. Convert GPIO to IRQ: ``irq_number = gpio_to_irq(GPIO)``.

3. Call ``request_irq()``

## Toggle LED by GPIO interrupt

**Features**:

* Toggle LED status by pressing button with interrupt. If pressed button for the first time, LED turn on, press one more time, LED turn off
* Count how many times the GPIO interrupt is triggered.

**Program**: [gpio_interrupt_toggle_led.c](gpio_interrupt_toggle_led.c)

After inserting the kernel module, ``/proc/interrupts`` will include GPIO 15:
```
           CPU0       CPU1       CPU2       CPU3
202:         96          0          0          0  pinctrl-bcm2835  15 Edge      Device name
```

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

# Platform driver to control GPIO

Check [this document and source code](https://github.com/TranPhucVinh/Raspberry-Pi-C/blob/main/Kernel/Device%20tree/Device%20tree%20GPIO.md) for a platform driver to control GPIO with GPIO node information parsed device tree overlay.
