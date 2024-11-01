import fcntl
import os
import struct
import ctypes
import array

SPI_DEV = "/dev/spidev0.0"  # /dev/spidev0.0 for SPI0, use GPIO08 for CS0
SPI_MODE_0  = 0 # SPI mode 0: (CPOL, CPHA) = (0, 0)
SPI_SPEED   = 500000
BITS_PER_WORD = 8
SEND_MSB_FIRST = 0
NO_DELAY = 0

SPI_IOC_WR_MODE             = 0x40016B01
SPI_IOC_WR_BITS_PER_WORD    = 0x40016b03
SPI_IOC_WR_MAX_SPEED_HZ     = 0x40046B04
SPI_IOC_RD_LSB_FIRST        = 0x80016b02
SPI_IOC_MESSAGE_1           = 0x40206b00 # SPI_IOC_MESSAGE(1)

class _CSpiIocTransfer(ctypes.Structure):
    _fields_ = [
        ('tx_buf', ctypes.c_ulonglong),
        ('rx_buf', ctypes.c_ulonglong),
        ('len', ctypes.c_uint),
        ('speed_hz', ctypes.c_uint),
        ('delay_usecs', ctypes.c_ushort),
        ('bits_per_word', ctypes.c_ubyte),
        ('cs_change', ctypes.c_ubyte),
        ('tx_nbits', ctypes.c_ubyte),
        ('rx_nbits', ctypes.c_ubyte),
        ('pad', ctypes.c_ushort),
    ]

class SPI_TX_Class(ctypes.Structure):
    _fields_ = [
        ("id", ctypes.c_int), 
        ("str_val", ctypes.c_char * 20)
    ]

def spi_transmit(spi_fd, tx_buf):
    buf = array.array('b', tx_buf) # Create an array in ASCII code
    buf_addr, buf_len = buf.buffer_info()

    spi_xfer = _CSpiIocTransfer()
    spi_xfer.tx_buf = buf_addr
    spi_xfer.len = buf_len
    spi_xfer.delay_usecs = NO_DELAY
    spi_xfer.speed_hz = SPI_SPEED
    spi_xfer.bits_per_word = BITS_PER_WORD
    
    fcntl.ioctl(spi_fd, SPI_IOC_MESSAGE_1, spi_xfer) # ioctl with SPI_IOC_MESSAGE(1)
    
def spi_bus_init():
    spi_fd = os.open(SPI_DEV, os.O_RDWR)
    
    fcntl.ioctl(spi_fd, SPI_IOC_WR_MODE, struct.pack("I", SPI_MODE_0)) # Set SPI mode 
    fcntl.ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, struct.pack("I", BITS_PER_WORD)) # Set bits per word
    fcntl.ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, struct.pack("I", SPI_SPEED)) # Set SPI speed
    fcntl.ioctl(spi_fd, SPI_IOC_RD_LSB_FIRST, struct.pack("I", SEND_MSB_FIRST)) # Set to send MSB first

    return spi_fd

def main():
    spi_fd = spi_bus_init()

    spi_tx_obj = SPI_TX_Class(456, b"Hello, World !")

    # Pack the object attributes into a byte buffer
    tx_data = bytearray()
    tx_data += struct.pack("i", spi_tx_obj.id)  # Pack integer id
    tx_data += spi_tx_obj.str_val  # Append string value as-is

    spi_transmit(spi_fd, tx_data)

    # ctypes.string_at() can be used
    # spi_tx_obj = SPI_TX_Class(456, b"Hello, World !")
    # spi_tx_obj_byte = ctypes.string_at(ctypes.addressof(spi_tx_obj), ctypes.sizeof(spi_tx_obj))
    # spi_transmit(spi_fd, spi_tx_obj_byte)
    
    print("Message sent successfully")

    os.close(spi_fd)

if __name__ == "__main__":
    main()
