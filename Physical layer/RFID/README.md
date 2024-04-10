MOSI and MISO byte order of MFRC522

| Line | Byte 0 |Byte 1 |Byte 2 | To|Byte n |Byte n+1 |
| ------- |:------:|:------:|:------:|:------:|:------:|:------:|
| MOSI | address 0 |address 1 |address 2 |...|address n |00 |
| MISO | (Don't care) |address 0's data |address 1's data |...|address n-1's data |address n's data |

Based on that transmission flow, in order to read the data on Address_0 register, we need a transmission buffer (tx_buf) of 2 members 
* tx_buf[0]: Store address 0
* tx_buf[1]: A dummy value

And a receiver buffer (rx_buf) of 2 members. Ater tx_buf[0] then tx_buf[1] are completely shifted from MOSI of master (Raspberry) to slave (MFRC522), rx_buf is already shifted into MISO of Raspberry. rx_buf[1] will then store Address_0 register's data.

The structure of any byte to store the register address of MFRC522 is

| 7 (MSB) | 6 to 1 |0 (LSB)|
| ------- |:------:|:------:|
| * 1 = read<br>* 0 = write | register address|address n |0 |

For example, to read the version register:

```c
uint8_t tx_buf[2];
tx_buf[0] = 0x80 | (VER_REG << 1) | 0x1;
tx_buf[1] = DUMMY_VALUE; // Dummy byte for receiving data
```

In **tx_buf[0]**:
* 0x80 = 0b10000000 (Bit 7th MSB)
* 0x1: Bit 0 (LSB)

**Implementation**: [read_ver_reg.c](read_ver_reg.c)
