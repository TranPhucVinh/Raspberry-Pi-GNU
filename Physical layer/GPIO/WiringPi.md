# Commands

All ``gpio`` commands are commands of ``WiringPi`` library.

All ``pin_name`` for gpio command are ``WiringPi`` pins, not ``BCM``.

* ``gpio -v``: Check version of current installed WiringPi library
* ``gpio mode pin_name value`` set up mode for ``pin_name``

``value``: ``in``, ``out``, ``pwm``, ``clock``, ``up``, ``down`` and ``tri``

* ``gpio write pin_name value``: Write value to ``pin_name`` 

``value``: ``0`` and ``1``

E.g:

```sh
$ gpio write 21 1
$ gpio write 21 0
```

* ``gpio read pin_name``: Read value from ``pin_name``
* ``gpio readall``: Read all GPIOs' value
* ``pinout``: List all pinout information

# Bash implementations

## Blink LED

```sh
#!/bin/bash
led=1
gpio mode $led out
while [ 1 ]
do
   gpio write $led 1
   sleep 0.5
   gpio write $led 0
   sleep 0.5
   echo "Pres CTRL+C to stop.0.."
done
```

## Use button to control LED

```sh
#!/bin/bash
led=1
button=15
gpio mode $led out
gpio mode $button in
while [ 1 ]
do
   gpio write $led $(gpio read $button)
   sleep 0.1
done
```

# C implementations

## Blink LED connected to WiringPi GPIO1

```c
#include <wiringPi.h>

#define LED 1

int main () {
    wiringPiSetup(); 
    pinMode(LED, OUTPUT);
    while(1) {
        digitalWrite (LED, HIGH);
        delay(1000);
        digitalWrite (LED, LOW);
        delay(1000);
    }
    return 0;
}
```

Blink LED with ``millis()``

```c
#include <wiringPi.h>

#define LED 1

unsigned int previousTime;
const unsigned long interval = 500;

int main() {
    wiringPiSetup(); 
    pinMode(LED, OUTPUT);
    previousTime = millis();
    
    while(1) {
        unsigned long diff = millis() - previousTime;
        if (diff > interval){
            digitalWrite(LED, !digitalRead(LED));
            previousTime += diff;
        }
    }
    return 0;
}
```

**Notice**:

With Raspbian, if you declare  ``unsigned int previousTime = millis();``, there will be an error:

```
initializer element is not constant
```

## Use button to control the LED: If button is pressed, turn on LED, if not pressed, turn off LED.

```c
#include <wiringPi.h>

#define LED 	1
#define BUTTON 	15

int main() {
    wiringPiSetup(); 
    pinMode (LED, OUTPUT);
    pinMode (BUTTON, INPUT);
    while(1) {
		digitalWrite(LED, digitalRead(BUTTON));
    }
    return 0;
}
```

## LED toggle

Press on the button the first time, LED will turn on; press the button next time, LED will turn off.

Using XOR operator

```c
#include <wiringPi.h>
#define LED 	1
#define BUTTON 	15

int pressed = 0, ledState = 0;

int main() {
    wiringPiSetup(); 
    pinMode (LED, OUTPUT);
    pinMode (BUTTON, INPUT);

    while(1) {
        if (!digitalRead(BUTTON) && !pressed){
            digitalWrite(LED, ledState);
            ledState ^= 1;
            pressed = 1;
        }
        else if (digitalRead(BUTTON) && pressed){
            pressed = 0;
        }    
    }
    return 0;
}
```