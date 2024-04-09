#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_DEV    "/dev/spidev0.0"// For /dev/spidev0.0, use GPIO08

int mode    = 0;        // A dummy variable for ioctl() to set write mode to SPI bus
int speed   = 500000;   // A dummy variable for ioctl() to set SPI master speed

void transfer(int fd, int *tx_buf, size_t len) {
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx_buf,
        .len = len,
        .speed_hz = speed
    };

    // As only define 1 struct spi_ioc_transfer so the totall message to sent is 1
    // So we use SPI_IOC_MESSAGE(1)
    if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 0) {
        perror("SPI transfer failed");
        exit(EXIT_FAILURE);
    }
}

int main() {
    int fd;
	fd = open(SPI_DEV, O_RDWR);
	if (fd < 0){
        printf("Unable to open %s\n", SPI_DEV);
        return 0;
    }

    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0){
        perror("Error when setting write mode to SPI device\n");
        close(fd);
    }

    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        perror("Error when setting SPI speed\n");
        close(fd);
    }

    char msg[] = "Hello, World ! from SPI master";
    size_t msg_length = sizeof(msg);

    int tx_buf[msg_length];

    memcpy(tx_buf, msg, msg_length);

    transfer(fd, tx_buf, msg_length);

    printf("Message sent successfully\n");

    close(fd);
}	