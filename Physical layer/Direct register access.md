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