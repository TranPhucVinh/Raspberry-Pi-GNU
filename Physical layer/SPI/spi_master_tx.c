#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_DEV    "/dev/spidev0.0"// /dev/spidev0.0 for SPI0, use GPIO08 for CS0
#define SPI_SPEED   500000

/*
	SPI mode (CPOL, CPHA)
 	0: (0, 0)
	1: (0, 1) 
 	2: (1, 0) 
  	3: (1, 1)  
*/
int mode    = 0;// SPI mode 0

void transfer(int fd, int *tx_buf, size_t len) {
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx_buf,
        .len = len,
        .speed_hz = SPI_SPEED
    };

    // As only define 1 struct spi_ioc_transfer so the total message to sent is 1
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

    char msg[] = "I've updated the speed";
    size_t msg_length = sizeof(msg);

    int tx_buf[msg_length];

    memcpy(tx_buf, msg, msg_length);

    transfer(fd, tx_buf, msg_length);

    printf("Message sent successfully\n");

    close(fd);
}	
