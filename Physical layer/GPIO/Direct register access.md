Bus address for peripherals advertised inside the official document like BCM2835, BCM2837 (e.g ``0x7E000000``) are mapped into the physical address (e.g: physical address starting at ``0x20000000`` for BCM2835)

Based on BCM2837-ARM-Peripherals, bus address of GPIO started at ``0x7E200000``:

```c
#define BCM2837_PHY_ADDR                0x3f000000 //BCM2837 physical address start
#define GPIO_BASE                       (BCM2837_PHY_ADDR + 0x200000)
```

To set up input/output mode based on GPIO Alternate function select register

Take an example with ``GPIO Alternate function select register 0``

| Bit(s) | Field Name|Description|Type|Reset|
| ------- |------|------|------|------|
|31-30 |--|Reserved|R|0|	
|29-27|FSEL9| FSEL9 - Function Select 9 <br> 000 = GPIO Pin 9 is an input <br> 001 = GPIO Pin 9 is an output <br> 100 = GPIO Pin 9 takes alternate function 0 <br> 101 = GPIO Pin 9 takes alternate function 1 <br> 110 = GPIO Pin 9 takes alternate function 2 <br> 111 = GPIO Pin 9 takes alternate function 3 <br> 011 = GPIO Pin 9 takes alternate function 4 <br> 010 = GPIO Pin 9 takes alternate function 5|R/W|0|