To config the system as password changing, enable I2C, SSH,...: ``sudo raspi-config``

The text editor of Raspbian is Geany: ``geany main.c``

# GPIO

The Raspberry Pi GPIO pins work with 3.3V logic levels and are not 5V tolerant, so a 5V to 3.3V logic converter is highly recommended when communicating with a 5V device.

# UART

All UARTs on the Raspberry Pi are 3.3V only - damage will occur if they are connected to 5V systems. An adaptor can be used to connect to 5V systems. 

Raspberry Pi 3 has two in-built UART which are as follows:

* PL011 UART (ttyAMA0)
* mini UART (ttyS0)

## Primary UART

On the Raspberry Pi 3 board, ``GPIO14 (UART0 TX)`` and ``GPIO15 (UART0 RX)`` are mini UART (``ttyS0``), also known as the primary UART. By default, this will also be the UART on which a Linux console may be present. Note that GPIO 14 is pin 8 on the GPIO header, while GPIO 15 is pin 10.

## Secondary UART

The secondary UART, also known as PL011 UART (``ttyAMA0``), is not normally present on the GPIO connector. By default, the secondary UART is connected to the on-board Bluetooth module of the combined wireless LAN/Bluetooth controller.

# I2C

Raspberry Pi 3 has 2 I2C port: I2C1 with pins GPIO3 (SCL.1) and GPIO2 (SDA.1) and I2C0.

By default I2C devices are disabled, they have to be enabled manually through ``raspi-config``.

For implementing I2C with Bash script with sensors and modules, check [I2C document](I2C.md).
# SPI
By default SPI devices are disabled, they have to be enabled manually through ``raspi-config``. After enabling, 2 devices ``/dev/spidev0.0`` and ``/dev/spidev0.1`` are available.

# ADC

Raspberry Pi 3 Model B+ doesn't have ADC module.

# Stream: Print out string

Print out a string after every 1 second. 

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main () {
     while(1) {
		//fprintf(stdout, "Hello, World!");
		printf("Hello, World!");
		fflush(stdout);
		sleep(1);
	}
    return 0;
}

```

**Notice**: 

* ``fflush(stdout)`` will empty the buffer of stdout for proper printing out every 1 second.
* For proper ``printf()``, ``fflush(stdout)`` must be followed after.

Without ``fflush(stdout)``, the program will be:

```c
while(1) {
	printf("Hello, World!");
	sleep(1);
}
```

This program will print out a bunch of strings at one time after a few seconds.

# Python

Python library ``RPi`` is built-in on Raspbian.
