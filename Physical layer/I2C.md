# Using system call ioctl

Check I2C sensors and modules examples in [Sensors and Modules](../Sensors%20and%20Modules).

# Python
Using library ``smbus`` (built-in library)

```py
import smbus

i2c_object = smbus.SMBus(I2C_port)
```

``I2C_port``: number, e.g ``0``, ``1``

E.g: ``i2c_object = smbus.SMBus(1)``

Raspberry Pi Python implementations:
* MLX90615
* PCF8574 HD44780 LCD 