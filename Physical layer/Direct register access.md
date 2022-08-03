# Fundamental concepts

BCM2835 contains the following peripherals which may safely be accessed by the ARM:

* Timers
* Interrupt controller
* GPIO
* USB
* PCM / I2S
* DMA controller
* I2C master
* I2C / SPI slave
* SPI0, SPI1, SPI2
* PWM
* UART0, UART1 

![](../Environment/Images/BCM2835_address_spaces.png)

Addresses in ARM Linux are:
* issued as virtual addresses by the ARM core, then
* mapped into a physical address by the ARM MMU, then
* mapped into a bus address by the ARM mapping MMU, and finally
* used to select the appropriate peripheral or location in RAM. 

Bus address for peripherals advertised inside the official document like BCM2835, BCM2837 (e.g ``0x7E000000``) are mapped into the physical address (e.g: physical address starting at ``0x20000000`` for BCM2835, ``0x3f000000`` for BCM2837). Thus a peripheral advertised here at bus address ``0x7Ennnnnn`` is available at physical address ``0x20nnnnnn``. 

## devmem

``busybox devmem`` is a tiny CLI utility (in busybox) that does ``mmap()`` in ``/dev/mem``.

``devmem`` will read 4 bytes (by default) from the physical address.

E.g: Read 4 bytes from physical address ``0x12345678``

```sh
sudo busybox devmem 0x12345678
```

### Working with ``devmem`` based on the virtual memory achieved by``iowrite32()`` operations

Kernel module program that setup output for GPIO and also print out the physical address and written value for later testing with ``devmem``:

```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/io.h>

#define BCM2837_PHY_ADDR                0x3f000000 //BCM2837 physical address start
#define GPIO_BASE                       (BCM2837_PHY_ADDR + 0x200000) /* GPIO controller */

#define LED			3

#define MAPPING_SIZE            0x100

MODULE_LICENSE("GPL");

unsigned int *gpio_base;

unsigned int *led_vir_address;

unsigned long led_phy_address;

unsigned long led_output_value = 0;//led_output_value to setup output mode, read, write operation

int init_module(void)
{
	printk(KERN_INFO "Hello, World !\n");

	//Memory mapping
	gpio_base = (unsigned int *)ioremap(GPIO_BASE, MAPPING_SIZE);
	if (gpio_base == NULL) printk("Couldn't perform mapping\n");
	else {
		led_phy_address = GPIO_BASE + LED/10;

		//Set virtual address for LED pin as the mapping with mmap()/ioremap() is linear
		led_vir_address = gpio_base + LED/10;

		//Set GPIO output
        led_output_value = ioread32(led_vir_address);
		led_output_value  &= ~(7<<(((LED)%10)*3));
		led_output_value |= (1<<(((LED)%10)*3));

		iowrite32(led_output_value, led_vir_address); //Set LED output

		printk("GPIO LED physical address 0x%lx\n", led_phy_address);//0x3f200000
		printk("GPIO LED virtual address 0x%p\n", led_vir_address);//0xdf1e40bc

		//Use hex value %x for easier comparing with devmem
		printk("led_output_value 0x%lx\n", led_output_value);//0x200
	}
		
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "clean up module\n");
	iounmap(gpio_base);
}
```

To check ``iowrite32(led_output_value, led_vir_address)``'s operation, use ``devmem`` for ``led_phy_address`` (GPIO LED physical address):

```sh
sudo busybox devmem 0x3f200000
```

This will return ``0x00000200``

Set output for LED with busy box

```sh
sudo busybox devmem 0x3f200000 w 0x00000200
```
