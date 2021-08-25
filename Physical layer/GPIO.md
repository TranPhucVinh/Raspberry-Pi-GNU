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

unsigned int previousTime;
const unsigned long interval = 1000;

int main (void) {
    wiringPiSetup(); 
    pinMode (21, OUTPUT);
    previousTime = millis();
    
    while(1) {
        unsigned long diff = millis() - previousTime;
        if (diff > interval){
            digitalWrite(21, !digitalRead(21));
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

Read button status and print out if status change.

```c
#include <stdio.h>
#include <wiringPi.h>

int main (void) {
    wiringPiSetup(); 
    pinMode (21, INPUT);
    int buttonStatus = digitalRead(21);
    printf("Button status at first %d ", buttonStatus);
    printf("");
    while(1) {
        if (digitalRead(21) != buttonStatus){
            printf("Button status %d ", buttonStatus);
            printf("");
            buttonStatus = digitalRead(21);
        }
    }
    return 0;
}
```

Use button to control LED: If button is pressed with HIGH level, LED will turn on; it not, LED will turn off.

```c
#include <stdio.h>
#include <wiringPi.h>

int main (void) {
    wiringPiSetup(); 
    pinMode (21, OUTPUT);
    pinMode (25, INPUT);
    int buttonStatus = digitalRead(25);
    while(1) {
        if (digitalRead(25) != buttonStatus){
            digitalWrite(21, digitalRead(25));
            buttonStatus = digitalRead(21);
        }
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