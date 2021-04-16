## Communication with slave

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

```c
#include <Arduino.h>
#include <Wire.h>

int count = 0;

void sendNumber();

void setup()
{
  Wire.begin(0x30); // Set up its I2C address to 0x30
  Wire.onRequest(sendNumber); // Set up received mode from Master board
}
 
void loop(){}
 
void sendNumber() 
{
 Wire.write(count);
 count++;
}
```

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

```c
#include <Arduino.h>
#include <Wire.h>

void receiveEvent();

void setup()
{
  Wire.begin(0x30); // Set up slave I2C address to 0x30
  Wire.onReceive(receiveEvent); // Set up received mode from Master board
  Serial.begin(9600);
}
 
void loop(){}
 
void receiveEvent() 
{
  while(Wire.available()) 
  {
    Serial.println(Wire.read());
  }
}
```
