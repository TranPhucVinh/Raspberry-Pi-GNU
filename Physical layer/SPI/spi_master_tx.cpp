#include <iostream>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_SPEED       500000
#define BITS_PER_WORD   8
#define SEND_MSB_FIRST  0
#define NO_DELAY        0

#define SPI_DEVICE "/dev/spidev0.0"

class SPIConfig
{
public:
    uint8_t mode;
    uint8_t bits_per_word;
    uint32_t speed_hz;
    uint16_t delay_usecs;
    uint16_t lsb_first;
};

class SPI {
    private:
        int _spi_fd;
        int _spi_mode;
        int _bpw; // bit per word
        int _speed;
        int _lsb_first;
        SPIConfig _spi_config;

    public:
        SPI(const char* spi_path);
        ~SPI();
        void set_config(SPIConfig *spiconfig);
        void write(void *tx_buf, size_t tx_buf_sz);
};

SPI::SPI(const char* spi_path){
    _spi_fd = open(spi_path, O_RDWR);
	if (_spi_fd < 0){
        perror("Unable to open SPI device\n");
        exit(0);
    }

    std::cout << "Open SPI device successfully\n";

    _spi_config.mode            = SPI_MODE_0;
    _spi_config.bits_per_word   = BITS_PER_WORD;
    _spi_config.speed_hz        = SPI_SPEED;
    _spi_config.lsb_first       = SEND_MSB_FIRST;
    _spi_config.delay_usecs     = NO_DELAY;

    set_config(&_spi_config);
}

void SPI::set_config(SPIConfig *spi_config){
    if (ioctl(_spi_fd, SPI_IOC_WR_MODE, &(spi_config->mode)) < 0){
        perror("Error when setting WR mode to SPI device\n");
        exit(0);
    }

    if (ioctl(_spi_fd, SPI_IOC_WR_BITS_PER_WORD, &(spi_config->bits_per_word)) < 0)
    {
        perror("Error when setting bit per word\n");
        exit(0);
    }

    if (ioctl(_spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &(spi_config->speed_hz)) < 0)
    {
        perror("Error when setting SPI speed\n");
        exit(0);
    }

    if (ioctl(_spi_fd, SPI_IOC_RD_LSB_FIRST, &(spi_config->lsb_first)) < 0)
    {
        perror("Error when setting SPI LSB first\n");
        exit(0);
    }
}

void SPI::write(void *tx_buf, size_t tx_buf_sz){
    spi_ioc_transfer spi_trans[1] = {};

    spi_trans[0].tx_buf     = (unsigned long)tx_buf;
    spi_trans[0].len        = tx_buf_sz;
    spi_trans[0].speed_hz   =  _spi_config.speed_hz;
    spi_trans[0].delay_usecs = _spi_config.delay_usecs;
    spi_trans[0].bits_per_word = _spi_config.bits_per_word;

    // As only define 1 struct spi_ioc_spi_transmit so the total message to sent is 1
    // So we use SPI_IOC_MESSAGE(1)
    if (ioctl(_spi_fd, SPI_IOC_MESSAGE(1), &spi_trans) < 0) {
        perror("SPI spi_transmit failed");
        exit(EXIT_FAILURE);
    }
}

SPI::~SPI()
{
    if (_spi_fd >= 0) close(_spi_fd);
}

int main() {
    SPI spi(SPI_DEVICE);

    char snd_msg[] = "spi_ioc_transfer spi_trans[1] = {};";
    spi.write((void*)snd_msg, sizeof(snd_msg));

    std::cout << "Message sent successfully\n";
}	
