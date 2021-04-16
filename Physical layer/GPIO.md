### Blink LED

Blink LED connected to GPIO21 of Raspberry Pi 2 second.

```c
#include <wiringPi.h>
int main (void) {
    wiringPiSetup (); 
    pinMode (21, OUTPUT);
    for (;;) {
        digitalWrite (21, HIGH);
        delay (2000);
        digitalWrite (21, LOW);
        delay (2000);
    }
    return 0;
}
```

### Blink LED with millis()

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

**Notice**: With Raspbian, if you declare  ``unsigned int previousTime = millis();``, there will be an error

**initializer element is not constant**

### Read GPIO status

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

### ON, OFF LED with button

If button is pressed with HIGH level, LED will turn off; it not (LOW level), LED will turn off.

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

### LED with bitwise

Press on button the first time, LED will turn on; press the button next time, LED turn off.

```c
#include <stdio.h>
#include <wiringPi.h>

int pressed = 0, toggle_val = 0;

int main (void) {
    wiringPiSetup(); 
    pinMode (21, OUTPUT);
    pinMode (25, INPUT);

    while(1) {
	if (digitalRead(25) == 0 && pressed == 0){
	    digitalWrite(21, toggle_val);
	    toggle_val ^= 1;
	    pressed = 1;
	}
	else if (digitalRead(25) == 1 && pressed == 1){
	    pressed = 0;
	}    
    }
    return 0;
}
```
