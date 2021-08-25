### Example 1

Blink LED connected to WiringPi GPIO1

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

### Example 2

Use button to control the LED: If button is pressed, turn on LED, if not pressed, turn off LED.

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

### Example 3

LED toggle: Press on the button the first time, LED will turn on; press the button next time, LED will turn off.

Using XOR operator

```c
#include <stdio.h>
#include <wiringPi.h>

int pressed = 0, ledState = 0;

int main (void) {
    wiringPiSetup(); 
    pinMode (21, OUTPUT);
    pinMode (25, INPUT);

    while(1) {
        if (digitalRead(25) && !pressed){
            digitalWrite(21, ledState);
            ledState ^= 1;
            pressed = 1;
        }
        else if (digitalRead(25) && pressed){
            pressed = 0;
        }    
    }
    return 0;
}
```