# I2C 

* [pcf8574.c](pcf8574.c): Control GPIO with PCF8574
* [pcf8574_hd44780_lcd.c](pcf8574_hd44780_lcd.c): PCF8574 for HD44780 LCD; PCF8574 HD44780 LCD Python library [PCF8574 HD44780 LCD](PCF8574%20HD44780%20LCD)
* [sht30.c](sht30.c): Read humidity and temperature of SHT30
* [DS1307](DS1307.md): hwclock command to read time of DS1307 which require its Raspbian built-in kernel module to be loaded
# MLX90615 UART
Connection: CP2102

When plug in with CP2102, sensor automatically send data to computer

Config port to 9600 ``0xA5 0xAE 0x53``

```py
import serial

arduino_serial = serial.Serial('/dev/ttyUSB0',9600)

arduino_serial.write(b'\xA5\xAE\x53')
```

Read 8 bytes

```py
while True:
    serial_receive = arduino_serial.read(8)
    print(serial_receive)
```    

## Read temperature

```py
import serial
import binascii
arduino_serial = serial.Serial('/dev/ttyUSB0',9600)

while True:
    serial_receive = arduino_serial.read(9)
    raw_data = binascii.hexlify(serial_receive).decode()
    split_text = raw_data.split('5a5a4504')#Split string for header value
    raw_temperature = split_text[1]
    to_high_str = "0x" + raw_temperature[0:2] #type: str
    to_low_str = "0x" + raw_temperature[2:4] #type: str
    to_high = int(to_high_str, 16) #convert to int then hex for >> operator
    to_low = int(to_low_str, 16)
    object_temperature = ((to_high << 8)|to_low)/100
    print("object_temperature", object_temperature)
```    

The output frequency is 10 Hz or cycle is 0.1 by default. ``time.sleep()`` when reading will result in delay temperature reading.
