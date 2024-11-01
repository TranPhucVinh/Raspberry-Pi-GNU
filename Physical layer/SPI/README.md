# API
## struct spi_ioc_transfer

This struct forms a struct for a SPI transaction.

* ``void *rx_buf``: Buffer for receive data
* **const void *tx_buf**: Buffer for transmit data
* **size_t len**: Length of receive/transmit buffers in bytes. If **len** bytes is transmit in the **tx_buf**, **len** bytes are received inside **rx_buf**
* **uint32_t speed_hz**: Set SPI master speed
## SPI_IOC_MESSAGE()
```c
#define SPI_IOC_MESSAGE(N) _IOWR(SPI_IOC_MAGIC, 0, struct spi_ioc_transfer[N])
```
**SPI_IOC_MESSAGE()** is used to transfer multiple SPI messages.

**Send 2 SPI message with SPI_IOC_MESSAGE(2)**
```c
struct spi_ioc_transfer transfer_arr[2];

char *msg0 = "SPI message 0", *msg1 = "SPI message 1";

struct spi_ioc_transfer transfer_0 = {
    .tx_buf = (unsigned long)msg0,
    .len = strlen(msg1),
    .speed_hz = speed
};

struct spi_ioc_transfer transfer_1 = {
    .tx_buf = (unsigned long)msg1,
    .len = strlen(msg1),
    .speed_hz = speed
};

transfer_arr[0] = transfer_0;
transfer_arr[1] = transfer_1;

if (ioctl(fd, SPI_IOC_MESSAGE(2), transfer_arr) < 0) {
    perror("SPI transfer failed");
    exit(EXIT_FAILURE);
}
```

# Implementations

**SPI master sends to SPI slave**: Raspberry Pi as the SPI master sends string to SPI slave connected on its SPI device, e.g /dev/spidev0.0. For ``/dev/spidev0.0``, we use GPIO08 (also called pin CE0):

* [spi_master_tx.c](spi_master_tx.c)
* [spi_master_tx.py](spi_master_tx.py)

**SPI master sends struct as a message to SPI slave**: 
* [spi_master_tx_struct.c](spi_master_tx_struct.c)
* [spi_master_tx_cstruct.py](spi_master_tx_cstruct.py): Use Python cstruct
* [spi_master_tx_class.py](spi_master_tx_class.py): Use class

For [spi_master_tx_cstruct.py](spi_master_tx_cstruct.py), **ctypes.string_at()** can be used:
```py
spi_tx_obj = SPI_TX_Class(456, b"Hello, World !")

spi_tx_obj_byte = ctypes.string_at(ctypes.addressof(spi_tx_obj), ctypes.sizeof(spi_tx_obj))
spi_transmit(spi_fd, spi_tx_obj_byte)
```

**SPI master reads response from SPI slave**: Raspberry Pi as SPI master sends REQ_SND, a flag number, to ESP32 as the SPI slave as a handshake. If slave receives REQ_SND from master, it will then send a string with index to master. Master receives that string and prints out. Program: [spi_master_rx.c](spi_master_rx.c)
