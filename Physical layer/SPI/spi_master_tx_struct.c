#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_DEV    "/dev/spidev0.0"// /dev/spidev0.0 for SPI0, use GPIO08 for CS0
#define SPI_SPEED   500000

struct _spi_rx_struct {
    int id;
    char str_val[20];
};

typedef struct _spi_rx_struct spi_rx_struct;

void spi_transmit(int spi_fd, void *tx_buf, size_t tx_buf_sz) {
    struct spi_ioc_transfer spi_trans = {
        .tx_buf     = (unsigned long)tx_buf,
        .len        = tx_buf_sz,
        .speed_hz   = SPI_SPEED
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
    int mode = 0;// SPI mode 0

    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) < 0){
        perror("Error when setting write mode to SPI device\n");
        close(spi_fd);
    }

    return spi_fd;
}

int main() {
    int spi_fd = spi_bus_init();

    spi_rx_struct rx_struct;
    rx_struct.id = 123;
    strcpy(rx_struct.str_val, "Hello, World ! 20");

    spi_transmit(spi_fd, (void*)&rx_struct, sizeof(rx_struct));

    printf("Message sent successfully\n");

    close(spi_fd);
}	