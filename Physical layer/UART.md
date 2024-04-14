# Bash implementation

## Raspberry sends a string to Linux computer

**Connection**: 5V, GND, TX, RX to USB TTL, then connects USB TTL to Linux computer.

**Raspberry**

Change mode of ``/dev/ttyS0``: ``sudo chmod 777 /dev/ttyS0``

Send string: ``echo "hello" > /dev/ttyS0``

**Linux computer**:``cat /dev/ttyUSB0``

## Linux computer sends a string to Raspberry

**Linux computer**: ``echo "string to send" > /dev/ttyUSB0``

**Raspberry**: ``cat /dev/ttyS0`` (read received data)

## Transmit data between Raspberry and Arduino

**Raspberry uses cat to read**: ``cat /dev/ttyS0``

## Raspberry send characters to control MCU GPIO

Check the corresponded document in Linux Shell: [Send characters from PC to MCU to control its GPIO
](https://github.com/TranPhucVinh/Linux-Shell/blob/master/Physical%20layer/USB.md#send-characters-from-pc-to-mcu-to-control-its-gpio)

``echo 9 > /dev/ttyUSB0``: Turn ON

``echo 8 > /dev/ttyUSB0``: Turn OFF

# Raspberry reads data from Arduino

Communicate between Raspberry Pi and Arduino Uno R3 using UART through a logic level converter.

Logic level converter module: 8 channel 5V-3.3V (3.3V channel A, 5V channel B)

| Uno R3 | Logic level converter |
| ------- |:------:|
| 5V   | 5V    |
| GND     | GND    |
| 2      | B7  |
| 3      | B6  |

| Raspberry Pi | Logic level converter |
| ------- |:------:|
| 3.3V   | 3.3V    |
| GND     | GND    |
| GPIO15 (TXD)      | A7  |
| GPIO16 (RXD)      | A6  |

**Arduino** sends a string every 1 second.

**Raspberry**

```c
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>

int main ()
{
    int fd;
    char *testString;

    //try to open Serial port with baudrate 115200
    if ((fd = serialOpen ("/dev/ttyS0", 115200)) < 0)
    {
        fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
        return 1 ;
    }

    wiringPiSetup();

    while (serialDataAvail(fd)){
        printf("%c", serialGetChar(fd));
        fflush (stdout) ;
    }
}    
```

To read data from ``/dev/ttyS0``: ``sudo chmod 777 /dev/ttyS0``

Then run the compiled program normally: ``./a.out``

# Raspberry sends data to Arduino

Raspberry sends a string to software serial of Arduino, Arduino receives this string and prints out by hardware serial.

**Raspberry**

```c
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>

int main ()
{
    int fd;
    char sendString[] = "Hello from Raspberry Pi";

    //try to open Serial port with baudrate 115200
    if ((fd = serialOpen ("/dev/ttyS0", 115200)) < 0)
    {
        fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
        return 1 ;
    }

    wiringPiSetup();

    while(1){
        for (int i=0;i<strlen(sendString);i++){
                serialPutchar(fd, sendString[i]);
        }
    }
    delay(1000);
}    
```
# Python
Use ``pyserial`` library.

## Read data from Arduino using UART pins

**Arduino**: Send ``Hello`` every 1 second baudrate 9600

**Raspberry**

```py
import serial, time

arduino_serial = serial.Serial("/dev/ttyS0", 9600)
while True:
	serial_receive = arduino_serial.readline()
	print(serial_receive)
```

## Send data to Arduino using UART pins

**Note**: ``write(data)`` requires the ``data`` to be ``byte`` type

```py
import serial, time

arduino_serial = serial.Serial("/dev/ttyS0", 9600)

while True:
	encoded_string = 'hello, World!'.encode('utf-8')
	arduino_serial.write(encoded_string)
	time.sleep(1)
```