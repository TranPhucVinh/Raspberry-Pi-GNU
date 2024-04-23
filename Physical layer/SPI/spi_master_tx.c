#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_DEV         "/dev/spidev0.0"// /dev/spidev0.0 for SPI0, use GPIO08 for CS0
#define SPI_SPEED       500000
#define BITS_PER_WORD   8
#define SEND_MSB_FIRST  0
#define NO_DELAY        0

void spi_transmit(int spi_fd, void *tx_buf, size_t tx_buf_sz) {
    struct spi_ioc_transfer spi_trans = {
        .tx_buf     = (unsigned long)tx_buf,
        .len        = tx_buf_sz,
        .speed_hz   = SPI_SPEED,
        .delay_usecs = NO_DELAY,
        .bits_per_word = BITS_PER_WORD,
    };

    // As only define 1 struct spi_ioc_spi_transmit so the total message to sent is 1
    // So we use SPI_IOC_MESSAGE(1)
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi_trans) < 0) {
        perror("SPI spi_transmit failed");
        exit(EXIT_FAILURE);
    }
}

int spi_bus_init(){
    int spi_fd;
	spi_fd = open(SPI_DEV, O_RDWR);
	if (spi_fd < 0){
        printf("Unable to open %s\n", SPI_DEV);
        return 0;
    }

    /*
        SPI mode (CPOL, CPHA)
        0: (0, 0)
        1: (0, 1) 
        2: (1, 0) 
        3: (1, 1)  
    */
    int mode = SPI_MODE_0;// SPI mode 0
    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0){
        perror("Error when setting write mode to SPI device\n");
        exit(0);
    }

    int bits_per_word = BITS_PER_WORD;
    if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word) < 0)
    {
        perror("Error when setting bit per word\n");
        exit(0);
    }

    int spi_speed = SPI_SPEED;
    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed) < 0)
    {
        perror("Error when setting SPI speed\n");
        exit(0);
    }

    int lsb_first = SEND_MSB_FIRST;
    if (ioctl(spi_fd, SPI_IOC_RD_LSB_FIRST, &lsb_first) < 0)
    {
        perror("Error when setting SPI LSB first\n");
        exit(0);
    }

    return spi_fd;
}

int main() {
    int spi_fd = spi_bus_init();

    char snd_msg[] = "Hello, World !";

    spi_transmit(spi_fd, (void*)snd_msg, sizeof(snd_msg));

    printf("Message sent successfully\n");

    close(spi_fd);
}	
