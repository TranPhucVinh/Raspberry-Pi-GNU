# Using system call ioctl

Check I2C sensors and modules examples in [Sensors and Modules](Sensors%20and%20Modules).

# Using wiringPi

**Connection**

| Raspberry Pi (Master) | Arduino (Slave) |
| ------- |:------:|
| 5V   | 5V    |
| GND     | GND    |
| SCL.1 (GPIO3)    | SCL (A5)  |
| SDA.1 (GPIO2) | SDA (A4)|

## Master receive data, Slave send data

### Master

```c
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

main() {
	int fd; //file description
	int data;

	wiringPiSetup();
	fd = wiringPiI2CSetup(0x30);
	if (fd == -1) {
		printf("No device found \n");
		return -1; // if not found device, return -1
	}
	else {
		while (1) {
			data = wiringPiI2CRead(fd);
			if (data == -1)
			{
				printf("No data return \n");
				return -1;
			}
			else {
				printf("data= %d \n", data);
			}
			delay(100);
		}
	}
	return 0;
}
```

### Slave

Check Slave program in ``Example 2`` of ``AVR-Arduino-framework/Physical layer/I2C/Examples part 1.md``.

## Master send data, Slave receives data

### Master

```c
#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

main() {
	int fd; //file description
	int data = 0;

	wiringPiSetup();
	fd = wiringPiI2CSetup(0x30);
	if (fd == -1) {
		printf("No device found \n");
		return -1;
	}
	else {
		while (1) {
			wiringPiI2CWrite(fd , data);
            data++;
			delay(1000);
		}
	}
	return 0;
}
```

### Slave

Check Slave program in ``Example 1`` of ``AVR-Arduino-framework/Physical layer/I2C/Examples part 1.md``.
