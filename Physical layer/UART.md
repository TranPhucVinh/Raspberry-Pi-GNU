### Raspberry reads data from Arduino

**Connection**

| Uno R3 | Raspberry Pi |
| ------- |:------:|
| 5V   | 5V    |
| GND     | GND    |
| RX      | GPIO14 (UART0 TX)  |
| TX      | GPIO15 (UART0 RX)  |

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

    while (serialDataAvail (fd)){
        printf("%c", serialGetChar(fd));
        fflush (stdout) ;
    }
}    
```

To read data from ``/dev/ttyS0``: ``sudo chmod 777 /dev/ttyS0``

Then run the compiled program normally: ``./a.out``

### Raspberry sends data to Arduino

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