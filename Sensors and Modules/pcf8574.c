#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> //for open()
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define I2C_BUS "/dev/i2c-1"
#define PCF8574	0x20
#define WRITE_SIZE	1

int main(int argc, char *argv[]) {
    int i2c_bus;

	i2c_bus = open(I2C_BUS, O_RDWR);
	if (i2c_bus < 0){
        printf("Unable to open I2C bus\n");
        return -1;
	}

    if (ioctl(i2c_bus, I2C_SLAVE, PCF8574) < 0){
        printf("Unable to perform ioctl for PCF8574 as I2C slave\n");
        close(i2c_bus);
        return -1;
    }

	int write_value[WRITE_SIZE] = {0x01};
	if (write(i2c_bus, write_value, WRITE_SIZE) != 1){
		printf("Unable to set GPIO for PCF8574\n");
		return -1;
	}
}	
