import RPi.GPIO as GPIO
import smbus
import http.client
import json, time
import lcd_driver

import board
import neopixel

RELAY_PIN = 4 #PIN 4 in BCM mode

GPIO.setmode(GPIO.BCM)
GPIO.setup(RELAY_PIN, GPIO.OUT)

MLX90615_I2C_ADDR = 0x5B

MLX90615_REG_TEMP_OBJECT = 0x27

mlx90615_i2c = smbus.SMBus(1)

#Try to connect to I2C LED
try:
	display = lcd_driver.lcd()
except:
	print("LCD is not connected or malfunction I2C bus")

pixels = neopixel.NeoPixel(board.D18, 30)

data = {
    "temperature": 0,
    "sensor_mac_address": 'b8:27:eb:81:5f:ed'
}

headers = {
    'Content-Type': 'application/json'
}

current_time = time.time()
real_temperature = 0.0

def http_client_send(real_temperature):
	data['temperature'] = real_temperature
	jsonData = json.dumps(data)
	#Try to connect to localhost HTTP server
	try:
		conn = http.client.HTTPConnection("localhost:5000")

		conn.request("POST", "/api/activity", jsonData, headers)
		r1 = conn.getresponse()
		return_data = r1.read()
		print("Return data: ", return_data.decode()) #Convert string to byte
	except:
		print("Unable to connect to HTTP server")

def object_temperature_read_and_display():
	global real_temperature

	#Try to read data from I2C sensor MLX90615
	try:
		temp_value = mlx90615_i2c.read_i2c_block_data(MLX90615_I2C_ADDR, MLX90615_REG_TEMP_OBJECT, 3)
		raw_temperature = temp_value[0]|(temp_value[1]<<8)
		real_temperature = raw_temperature*0.02 - 273.15
		display.lcd_display_string(str(round(real_temperature, 2)), 1)
		
		#If temp exceed 38, send temp to HTTP_server
		if (real_temperature >= 38):
			GPIO.output(RELAY_PIN, GPIO.HIGH)
			pixels.fill((32,0,0))
			http_client_send(round(real_temperature, 2))
		elif (real_temperature >= 36) and (real_temperature < 38):
			GPIO.output(RELAY_PIN, GPIO.LOW)
			pixels.fill((0,32,0))
		else:
			GPIO.output(RELAY_PIN, GPIO.LOW)
			pixels.fill((0,0,32))
	except:
		print("Sensor is not connected or malfunction I2C bus")	

while True:
	object_temperature_read_and_display()
	time.sleep(0.5)
	display.lcd_clear()

	#Send data to HTTP_server every 30 seconds
	# if (time.time() - current_time >  30):
	# 	http_client_send(round(real_temperature, 2))
	# 	current_time = time.time()