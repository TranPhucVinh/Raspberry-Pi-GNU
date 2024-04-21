#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_DEV    "/dev/spidev0.0"// /dev/spidev0.0 for SPI0, use GPIO08 for CS0
#define SPI_SPEED   500000

void spi_transmit(int spi_fd, void *tx_buf, size_t tx_buf_sz) {
    struct spi_ioc_transfer spi_tx = {
        .tx_buf     = (unsigned long)tx_buf,
        .len        = tx_buf_sz,
        .speed_hz   = SPI_SPEED
    };

    // As only define 1 struct spi_ioc_spi_transmit so the total message to sent is 1
    // So we use SPI_IOC_MESSAGE(1)
    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi_tx) < 0) {
        perror("SPI spi_transmit failed");
        exit(EXIT_FAILURE);
    }
}

void spi_receive(int spi_fd, void *rx_buf, size_t rx_buf_sz){
    int dummy_spi_tx = 1;

    struct spi_ioc_transfer spi_rx = {
        .tx_buf     = (unsigned long)&dummy_spi_tx,
        .rx_buf     = (unsigned long)rx_buf,
        .len        = rx_buf_sz,
        .speed_hz   = SPI_SPEED
    };

    if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi_rx) < 0) {
        perror("SPI spi_receive failed");
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
        close(spi_fd);
    }

    return spi_fd;
}

int main() {
    int spi_fd = spi_bus_init();

    uint8_t REQ_SND = 0x12; //A flag with dummy value to request SPI slave to send message to master

    spi_transmit(spi_fd, (void*)&REQ_SND, sizeof(REQ_SND));

    printf("Send request message to SPI slave successfully\n");

    char rx_buf[50];

    spi_receive(spi_fd, (void*)rx_buf, sizeof(rx_buf));
    printf("rx_buf: %s\n", rx_buf);
    
    close(spi_fd);
}	
