#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h> //for open()
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define I2C_BUS "/dev/i2c-1"
#define SHT30	0x44

// #define PERIODIC_MODE
#define SINGLE_MODE

#if defined(SINGLE_MODE)
//Single-shot High Repeatability, enable Clock stretching
#define MSB_COMMAND 0x2c
#define LSB_COMMAND 0x06
#elif defined(PERIODIC_MODE)
//Preriodic mode, 2mps, medium
#define MSB_COMMAND 0x22
#define LSB_COMMAND 0x20
#endif

#define READ_SIZE	6
#define STORE_SIZE	2

char sht30_data[READ_SIZE];
uint8_t sht30_temperature[STORE_SIZE];
uint8_t sht30_humidity[STORE_SIZE];

uint8_t sender_crc_calculate(uint8_t *one_wire_frame, uint8_t size);

int main(int argc, char *argv[]) {
    int i2c_bus;

	i2c_bus = open(I2C_BUS, O_RDWR);
	if (i2c_bus < 0){
        printf("Unable to open I2C bus\n");
        return -1;
	}

    if (ioctl(i2c_bus, I2C_SLAVE, SHT30) < 0){
        printf("Unable to perform ioctl for SHT30 as I2C slave\n");
        close(i2c_bus);
        return -1;
    }

    char write_command[2] = {MSB_COMMAND, LSB_COMMAND};
    int ret = write(i2c_bus, write_command, 2);
    if (ret != 2){
        printf("Unable to write command to SHT30, ret %d\n", ret);
        close(i2c_bus);
        return -1;
    }

#if defined(SINGLE_MODE)
    char temp[2] = {MSB_COMMAND, LSB_COMMAND};
#elif defined(PERIODIC_MODE)
    char temp[2] = {0xe0, 0x00};
#endif
    while (1){
        int rett = write(i2c_bus, temp, 2);
        usleep(10000);
        if (read(i2c_bus, sht30_data, READ_SIZE) < 0){
            printf("Unable to read SHT30 temperature and humidity\n");
            break;
        } else {
            sht30_temperature[0] = sht30_data[0];
            sht30_temperature[1] = sht30_data[1];

            if (sender_crc_calculate(sht30_temperature, STORE_SIZE) == sht30_data[2]){
                int raw_temperature = (sht30_temperature[0]<<8) + sht30_temperature[1]; 
                float temperature = -45 + (float)(175*(raw_temperature/(pow(2, 16) - 1)));
                printf("Environment temperature: %lf*C\n", temperature);
            }

            sht30_humidity[0] = sht30_data[3];
            sht30_humidity[1] = sht30_data[4];

            if (sender_crc_calculate(sht30_humidity, STORE_SIZE) == sht30_data[5]){
            /*
                    Use uint16_t to avoid overflowing.
                    Using int will be overflowing and raw_humidity will be negative
                */	
                uint16_t raw_humidity = (sht30_humidity[0]<<8) + sht30_humidity[1];
                float humidity = (float)(100*(raw_humidity/(pow(2, 16) - 1)));
                printf("Environment humidity: %lf%%\n", humidity);
            }
        }
        sleep(1);
    }
}	

uint8_t sender_crc_calculate(uint8_t *one_wire_frame, uint8_t size){
	uint8_t crc = 0xff;
	for (size_t i = 0; i < size; i++) {
		crc ^= one_wire_frame[i];
		for (size_t j = 0; j < 8; j++) {
			if ((crc & 0x80) != 0) {
				crc = ((crc << 1u) ^ 0x31);
			} else {
				crc <<= 1u;
			}
		}
	}
	return crc;
}
