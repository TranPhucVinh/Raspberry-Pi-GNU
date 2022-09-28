## GPIO control

Kernel module as driver to control GPIO:
* [blink_led.c](blink_led.c): Blink LED by GPIO kernel API and kernel timer setup
* [control_led_with_button.c](control_led_with_button.c): Use button to control the LED. If button is pressed, turn on LED, if not pressed, turn off LED.

Control GPIO with ``linux/io``:

* [blink_led_with_ioremap.c](blink_led_with_ioremap.c): Blink LED with ``ioremap()``
* [blink_led_with_devm_ioremap.c](blink_led_with_ioremap.c): Blink LED with ``devm_ioremap()`` (for character device)
* [blink_led_with_iowrite32.c](blink_led_with_iowrite32.c): Blink LED with ``ioremap()`` and ``iowrite32()``
* [control_led_with_ioread32.c](control_led_with_ioread32.c): Use button to control the LED. If button is pressed, turn on LED, if not pressed, turn off LED. Using ``ioread32()`` and ``iowrite32()``.

### API

The ``ioremap`` function takes two parameters:

* start of the memory region
* size of the memory region

```c
void *ioremap(unsigned long phys_addr, unsigned long size);
void iounmap(void * addr);
```

Corresponded functions for character devices:

```c
void __iomem *devm_ioremap(struct device *dev, resource_size_t offset, resource_size_t size);
void devm_iounmap(struct device *dev, void __iomem *addr);
```

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

**Note**: Without ``gpio_direction_output()`` setup, there will be error:

```
[ 5808.116545] gpio gpiochip0: (pinctrl-bcm2835): gpiochip_lock_as_irq: cannot get GPIO direction
[ 5808.116566] gpio gpiochip0: (pinctrl-bcm2835): unable to lock HW IRQ 2 for IRQ
[ 5808.116582] genirq: Failed to request resources for GPIO_IRQ (irq 199) on irqchip pinctrl-bcm2835
[ 5808.116597] Can't request interrupt number 199
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