#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_DEV         "/dev/spidev0.0"
#define SPEED           500000

int mode    = 0;        // A dummy variable for ioctl() to set write mode to SPI bus

#define VER_REG         0x37
#define DUMMY_VALUE     0x00

void request_msg_from_spi_slave(int fd) {
    uint8_t tx_buf[2];
    tx_buf[0] = 0x80 | (VER_REG << 1) | 0x1; // Command byte (0x80 for read, 0x37 for version register)
    tx_buf[1] = DUMMY_VALUE; // Dummy byte for receiving data
    uint8_t rx_buf[2]; // Buffer for receiving data

    struct spi_ioc_transfer spi_trans = {
        .tx_buf = (unsigned long)tx_buf,
        .rx_buf = (unsigned long)rx_buf,
        .len = 2,
        .speed_hz = SPEED
    };

    if (ioctl(fd, SPI_IOC_MESSAGE(1), &spi_trans) < 0) {
        perror("SPI transfer failed");
        exit(EXIT_FAILURE);
    }

    // Extract version data from received buffer
    uint8_t version = rx_buf[1];

    printf("0x%x 0x%x\n", rx_buf[0], rx_buf[1]);
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

    request_msg_from_spi_slave(fd);

    close(fd);
}	
