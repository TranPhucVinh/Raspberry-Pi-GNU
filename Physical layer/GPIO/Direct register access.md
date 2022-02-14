# Fundamental concepts

Based on BCM2837-ARM-Peripherals, bus address of GPIO started at ``0x7E200000``:

```c
#define BCM2837_PHY_ADDR                0x3f000000 //BCM2837 physical address start
#define GPIO_BASE                       (BCM2837_PHY_ADDR + 0x200000)
```

## Set up input/output mode based on GPIO Alternate function select register

Take an example with **GPIO Alternate function select register 0**

| Bit(s) | Field Name|Description|Type|Reset|
| ------- |------|------|------|------|
|31-30 |--|Reserved|R|0|	
|29-27|FSEL9| FSEL9 - Function Select 9 <br> 000 = GPIO Pin 9 is an input <br> 001 = GPIO Pin 9 is an output <br> 100 = GPIO Pin 9 takes alternate function 0 <br> 101 = GPIO Pin 9 takes alternate function 1 <br> 110 = GPIO Pin 9 takes alternate function 2 <br> 111 = GPIO Pin 9 takes alternate function 3 <br> 011 = GPIO Pin 9 takes alternate function 4 <br> 010 = GPIO Pin 9 takes alternate function 5|R/W|0|
|26-24|FSEL8|FSEL8 - Function Select 8|R/W|0|
|23-21|FSEL7|FSEL7 - Function Select 7|R/W|0|
|20-18|FSEL6|FSEL6 - Function Select 6|R/W|0|
|17-15|FSEL5|FSEL6 - Function Select 5|R/W|0|
|14-12|FSEL4|FSEL4 - Function Select 4|R/W|0|
|11-9|FSEL3|FSEL3 - Function Select 3|R/W|0|
|8-6|FSEL2|FSEL2 - Function Select 2|R/W|0|
|5-3|FSEL1|FSEL1 - Function Select 1|R/W|0|
|2-0|FSEL0|FSEL0 - Function Select 0|R/W|0|

As 3-bit is used to set the input/output mode for every GPIO pin:

* Use ``7``  (``0b111``) then execute complement operator (``~``) to change to ``0b000`` for input mode.
* First setting up bit like input mode to clear 3-bit, then use ``1`` (``0b001``) then OR bit for output mode.

As the position of those 3-bit varies from GPIO pin, so ``( GPIO_pin_number % 10) * 3`` will result in the exact position of those pins:

E.g:

GPIO pin 9: ``(9 % 10) * 3 = 27``

GPIO pin 8: ``(8  % 10) * 3 = 24``

Then start shifting for the exact position of those 3-bit corresponded to the specific GPIO pin:

GPIO pin 9: ``7 << 27``

Then:

* Start executing complement operator to switch those 3-bit to ``0b000`` for input mode.
* Start the whole process like input mode then use OR operator for putput mode.

So the whole process will be: 

* AND GPIO register with ``~(7<<(((pin)%10)*3))`` for input mode
* AND GPIO register with ``~(7<<(((pin)%10)*3))`` then OR GPIO register with ``(1<<(((pin)%10)*3))`` for output mode

Take ``addr`` as the virtual address mapped from the ``GPIO_BASE`` physical address, then the virtual address of the specific GPIO pin will be: ``(addr+((pin)/10))``

E.g: 

For GPIO ``0`` to ``9``: ``pin/10 = 0`` then ``addr+((pin)/10)`` will be the virtual address mapped from ``GPFSEL0`` physical address (``0x7E200000``)

For GPIO ``10`` to ``19``: ``pin/10 = 1`` then ``addr+((pin)/10)`` will be the virtual address mapped from ``GPFSEL1`` physical address (``0x7E200004``)

As the virtual address mapping process, all registers ``GPFSEL0``, ``GPFSEL1``,...  with have the virtual address arranged continuously (2 consecutively registers like ``GPFSEL0`` and ``GPFSEL1`` will have the address difference by ``1``)

So for GPIO input mode setup:

```c
#define GPIO_INP(addr,pin) *(addr+((pin)/10)) &= ~(7<<(((pin)%10)*3))
```

GPIO output mode setup:

```c
#define GPIO_OUT(addr,pin) {\
        *(addr+((pin)/10)) &= ~(7<<(((pin)%10)*3));\
        *(addr+((pin)/10)) |= (1<<(((pin)%10)*3));\
}
```

### Set, clear and get GPIO value

``GPSETn`` is used to set the value of a specific GPIO. ``GPSET0`` is available for GPIO from ``0`` to ``31``.

``1`` = Set GPIO pin n

As ``GPSET0`` is ``7`` registers far from the ``GPIO_BASE``:

```c
#define GPIO_SET(addr,pin) *(addr+7)=1<<pin //Set GPIO in GPSET0
```

Same mechanism for ``GPCLR0`` and ``GPLEV0``:

```c
#define GPIO_CLR(addr,pin) *(addr+10)=1<<pin //Clear GPIO in GPCLR0
#define GPIO_GET(addr,pin) ((*(addr+13))>>pin)&1 //GET GPIO value in GPLEV0
```

# Examples

``direct_register_access_control_gpio.c``: Control GPIO by direct register access in BCM2537. To run the program: ``sudo ./a.out`` (to open ``/dev/mem``).