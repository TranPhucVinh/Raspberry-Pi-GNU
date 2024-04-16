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
# MLX90615 I2C

Read MLX90615 I2C temperature sensor: Read object temperature
 
```py
import time
import smbus

MLX90615_I2C_ADDR = 0x5B

MLX90615_REG_TEMP_OBJECT = 0x27

mlx90615_i2c = smbus.SMBus(1)

while True:
	temp_value = mlx90615_i2c.read_i2c_block_data(MLX90615_I2C_ADDR, MLX90615_REG_TEMP_OBJECT, 3)
	
	raw_temperature = temp_value[0]|(temp_value[1]<<8)
	real_temperature = raw_temperature*0.02 - 273.15
	print(real_temperature)
	
	time.sleep(0.1)
```	
Beside that, MLX90615 also have UART module. Refer to [MLX90615 UART document](https://github.com/TranPhucVinh/Python/blob/master/Platforms%20interaction/Raspberry%20Pi/UART.md#mlx90615-uart) for its implementation.
