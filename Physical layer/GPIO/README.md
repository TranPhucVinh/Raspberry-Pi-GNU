# sysfs

GPIO chip like ``gpiochip0``, ``gpiochip100``, ``gpiochip504`` allow user to create GPIO virtual file.

Create a GPIO virtual file:

```sh
cd /sys/class/gpio/
echo 14 > export #create GPIO14 virtual file
```

For invalid GPIO virtual file name, e.g: ``echo a > export``, there will be error:

```c
-bash: echo: write error: Invalid argument
```

Then, inside ``/sys/class/gpio/``, there will be:

```sh
export  gpio14  gpiochip0  gpiochip100  gpiochip504  unexport
```

Inside ``/sys/class/gpio/gpio14``, there will be:

```sh
active_low  device  direction  edge  power  subsystem  uevent  value
```

```sh
pi@raspberrypi:/sys/class/gpio/gpio14 $ echo out > direction #set up output for gpio14
pi@raspberrypi:/sys/class/gpio/gpio14 $ echo 1 > value #set 1 for GPIO14
pi@raspberrypi:/sys/class/gpio/gpio14 $ echo 0 > value #set 0 for GPIO14
```

After powering off, ``gpio14`` will be deleted.

## Bash script to blink LED

```sh
led=14
path=/sys/class/gpio/

echo $led > $path"export"
echo out > $path"gpio"$led"/direction"

while true;
do
   echo 1 > $path"gpio"$led"/value"
   sleep 1
   echo 0 > $path"gpio"$led"/value"
   sleep 1
done
```

## C program to blink LED

Before running the program, export GPIO14 to sysfs:

```sh
pi@raspberrypi:~ $ echo 14 > /sys/class/gpio/export
```

**Program**: [sysfs_blink_led.c](sysfs_blink_led.c)
## C program to read button status

Use GPIO 15 as button: 
```sh
pi@raspberrypi:~ $ echo 15 > /sys/class/gpio/export
```
**Program**: [sysfs_read_button.c](sysfs_read_button.c)

## C program to control LED by button

Press on button to turn on LED, unpress button to turn off LED: [sysfs_button_control_led.c](sysfs_button_control_led.c)

# Direct register access

Check [Direct register access document](Direct%20register%20access.md)

# Python
``GPIO.setmode(mode)`` has acceptance mode are ``GPIO.BOARD`` and ``GPIO.BCM``
**Blink LED**

```py
import time
import RPi.GPIO as GPIO

LED_PIN = 7 #GPIO 7 on board

GPIO.setmode(GPIO.BOARD)
GPIO.setup(LED_PIN, GPIO.OUT)

while True:
	GPIO.output(LED_PIN, GPIO.HIGH)
	time.sleep(1)
	GPIO.output(LED_PIN, GPIO.LOW)
	time.sleep(1)
```	