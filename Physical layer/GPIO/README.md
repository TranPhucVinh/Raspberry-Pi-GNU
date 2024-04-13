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

## Bash script to control GPIO

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

## [C program for sysfs to blink LED](sysfs_blink_led.c)

# Direct register access

Check [Direct register access document](Direct%20register%20access.md)

# WiringPi

Check [WiringPi.md](https://github.com/TranPhucVinh/Raspberry-Pi-C/blob/main/Physical%20layer/GPIO/WiringPi.md)