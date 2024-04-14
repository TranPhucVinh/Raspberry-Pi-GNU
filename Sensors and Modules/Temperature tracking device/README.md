# Feature

* Using Python core
* Raspberry Pi read temperature from MLX90615 I2C temperature sensor
* I2C LCD 16x02 displays temperature
* LED RGB indicates temperature status
    * temperature >= 38: RED
    * 38 > temperature > 35: GREEN
    * temperature < 35: BLUE

* Relay to control buzzer if temperature exceed 38
* HTTP clients:
    * Send temperature every 30 seconds follow REST API message structure (keep alive message)
    * If temperature is over threshold value, send message immediately

# REST API

**Request**

```
api: hostname/api/activity
Body: 
{
    "temperature": 10.5,
    "sensor_mac_address": "10:10:10:10:10:10"
}
```

**Response**

```
Status code: 200
Body: 
{
    "msg": "success"
}
```

# Description

* Main program: ``main.py``, run: ``sudo python3 main.py``
* LCD library: [PCF8574 HD44780 LCD](https://github.com/TranPhucVinh/Python/tree/master/Platforms%20interaction/Raspberry%20Pi/PCF8574%20HD44780%20LCD)
* Library for LED ring WS2812B: ``sudo pip3 install rpi_ws281x adafruit-circuitpython-neopixel``
* HTTP server to test: ``http_server.py``

# Connection

| Pin | Raspberry |
| ------- |:------:|
| RELAY  | GPIO 07 (BCM 04)    |
| LED RGB | GPIO 18 (BCM 18)    |
| SDA | GPIO 02    |
| SCL | GPIO 03   |

# Appendix

## WS2812B LED RGB ring

Download: ``sudo pip3 install rpi_ws281x adafruit-circuitpython-neopixel``

Module: WS2812B 16 LED ring

**Wiring**

| Raspberry Pi | WS2812B LED ring |
| ------- |:------:|
| 5V   | 5V    |
| GND     | GND    |
| GPIO7 (BCM GPIO4)|DI|

Run code: ``sudo python3 test.py``

WS2812B runs with BCM mode so other PIN out has to use BCM mode to be compatible with WS2812B pins.

**Example**: Light up 10 LED with RED

```py
import board
import neopixel
import time

pixels = neopixel.NeoPixel(board.D18, 30)

for x in range(0,9):
	pixels[x] = (255,0,0)
	time.sleep(1)

for x in range(0,9):
	pixels[x] = (0,0,0)
	time.sleep(1)
```	

Notice that WS2812B use BMC mode, not GPIO mode, so for GPIO mode usage:

take this example:

```py
import board
import neopixel
import time
import RPi.GPIO as GPIO

pixels = neopixel.NeoPixel(board.D18, 30)

pixels[0] = (255,0,0)

#RELAY_PIN = 7 #GPIO 7 on board
RELAY_PIN = 4 #MODE BCM: BOARD GPIO7

GPIO.setmode(GPIO.BCM)
GPIO.setup(RELAY_PIN, GPIO.OUT)

for x in range(0,9):
	pixels[x] = (255,0,0)
	time.sleep(1)

for x in range(0,9):
	pixels[x] = (0,0,0)
	time.sleep(1)


while True:
	GPIO.output(RELAY_PIN, GPIO.HIGH)
	time.sleep(1)
	GPIO.output(RELAY_PIN, GPIO.LOW)
	time.sleep(1)
```

**Blink the whole LED string RED, GREEN, BLUE**

```py
import board
import neopixel
import time

pixels = neopixel.NeoPixel(board.D18, 30)

pixels[0] = (255,0,0)

for x in range(0,9):
	pixels.fill((255,0,0))
	time.sleep(1)
	pixels.fill((0,255,0))
	time.sleep(1)
	pixels.fill((0,0,255))
	time.sleep(1)
```
