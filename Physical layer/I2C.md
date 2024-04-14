# Using system call ioctl

Check I2C sensors and modules examples in [Sensors and Modules](../Sensors%20and%20Modules).

# Python
Using library ``smbus`` (built-in library)

```py
import smbus

i2c_object = smbus.SMBus(I2C_port)
```

``I2C_port``: number, e.g ``0``, ``1``

E.g: ``i2c_object = smbus.SMBus(1)``
# MLX90615 I2C

Read MLX90615 I2C temperature sensor: Read object temperature
 
```py
import time
import smbus

MLX90615_I2C_ADDR = 0x5B

MLX90615_REG_TEMP_OBJECT = 0x27

mlx90615_i2c = smbus.SMBus(1)

while True:
	temp_value = mlx90615_i2c.read_i2c_block_data(MLX90615_I2C_ADDR, MLX90615_REG_TEMP_OBJECT, 3)
	
	raw_temperature = temp_value[0]|(temp_value[1]<<8)
	real_temperature = raw_temperature*0.02 - 273.15
	print(real_temperature)
	
	time.sleep(0.1)
```	
Beside that, MLX90615 also have UART module. Refer to [MLX90615 UART document](https://github.com/TranPhucVinh/Python/blob/master/Platforms%20interaction/Raspberry%20Pi/UART.md#mlx90615-uart) for its implementation.
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

[Arduino I2C slave program](https://github.com/TranPhucVinh/AVR-Arduino-framework/blob/master/Physical%20layer/Wired%20communication/I2C/Examples.md#slave) for [Master sends data, Slave receives data](https://github.com/TranPhucVinh/AVR-Arduino-framework/blob/master/Physical%20layer/Wired%20communication/I2C/Examples.md#master-sends-data-slave-receives-data).
