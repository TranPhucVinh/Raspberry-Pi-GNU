Each GPIO pin, when configured as a general-purpose input, can be configured as an interrupt source.

### API

```c
int wiringPiISR (int pin, int edgeType,  void (*function)(void));
```

* ``edgeType``: ``INT_EDGE_FALLING``, ``INT_EDGE_RISING``, ``INT_EDGE_BOTH`` or ``INT_EDGE_SETUP``.

### Print out with interrupt

Print out a string if button is pressed as an interruption while blinking LED.

```c
#include <stdio.h>
#include <wiringPi.h>

void interruptFunction(){
    printf("Enter interrupt mode");
    fflush(stdout);
}   

int main (void) {
    wiringPiSetup(); 
    pinMode (21, OUTPUT);
    pinMode (25, INPUT);

    wiringPiISR(25, INT_EDGE_FALLING, &interruptFunction);
    
    while(1) {
        digitalWrite (21, HIGH);
        delay (2000);
        digitalWrite (21, LOW);
        delay (2000);
    }
    return 0;   
}
```

### ON/OFF LED with Interrupt

If button is pressed, LED is ON; if not, LED is off.

```c
#include <stdio.h>
#include <wiringPi.h>

int ledState = LOW;

void interruptFunction(){
    if (ledState == LOW) {
        ledState = HIGH;
    } else {
        ledState = LOW;
    }    
    digitalWrite(21, ledState);
}   

int main (void) {
    wiringPiSetup(); 
    pinMode (21, OUTPUT);
    pinMode (25, INPUT);

    wiringPiISR(25, INT_EDGE_FALLING, &interruptFunction);
    
    while(1);
    return 0;
}
```
