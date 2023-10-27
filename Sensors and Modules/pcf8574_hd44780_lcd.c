/*
 *  PCF8574T     LCD HD44780                                               *
 *  ========     ======                                                    *
 *     P0        RS                                                        *
 *     P1        RW                                                        *
 *     P2        Enable                                                    *
 *     P3        Led Backlight                                             *
 *     P4        D4                                                        *
 *     P5        D5                                                        *
 *     P6        D6                                                        *
 *     P7        D7                                                        *
 *                                                                         *
 * I2C-byte: D7 D6 D5 D4 BL EN RW RS                                       *
 * ----------------------------------------------------------------------- */

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define I2C_BUS "/dev/i2c-1"

#define PCF8574	0x27
#define WRITE_SIZE	1

//Function set register
#define FUNCTIONSET_DB5 		0x20
#define FUNCTIONSET_4BITMODE 	0x00
#define FUNCTIONSET_2LINE 		0x08
#define FUNCTIONSET_1LINE 		0x00
#define FUNCTIONSET_5x10DOTS 	0x04
#define FUNCTIONSET_5x8DOTS		0x00

//Display control register
#define DISPLAYCTRL_DB3			0x08
#define DISPLAYCTRL_DISPLAYON	0x04
#define DISPLAYCTRL_DISPLAYOFF 	0x00
#define DISPLAYCTRL_CURSORON 	0x02
#define DISPLAYCTRL_CURSOROFF 	0x00
#define DISPLAYCTRL_BLINKON 	0x01
#define DISPLAYCTRL_BLINKOFF	0x00

//Entry mode register
#define ENTRYMODE_DB2 				0x04
#define ENTRYMODE_DECREMENTS 		0x00
#define ENTRYMODE_INCREMENTS 		0x02
#define ENTRYMODE_SHIFTINCREMENT 	0x01
#define ENTRYMODE_SHIFTDECREMENT 	0x00

//Cursor shift register
#define CURSORSHIFT_DB4                       0x10
#define CURSORSHIFT_MOVE_LEFT                 0x00
#define CURSORSHIFT_DISPLAY_LEFT              0x04

#define LCD_CLEARDISPLAY 		0x01
#define LCD_RETURNHOME			0x02

//Set DDRAM address
#define SETDDRAMADDR_2_LINE 0x80
#define SETDDRAMADDR_1_LINE 0x00

#define RS_HIGH             0x01
#define RS_LOW              0x00

#define EN_HIGH             0x04
#define EN_LOW              0x00

#define BL_ON               0b1000 // Backlight on

//user
#define USER_COMMAND 0x00
#define USER_WRITE   0x01

#define COLUMNS  20
#define ROWS    4

int  _ENTRYMODE;
int  _DISPLAYCTRL;
int  _CURSORSHIFT;
int  _FUNCTIONSET;

int i2c_fd;//I2C file descriptor

void write_4bits(uint8_t value, int func);
void i2c_write_8bits(uint8_t data);
void lcd_write_char(char character);
void lcd_write_string(char *string);
void set_cursor(int column, int row);

int i2c_bus_start(const char *i2c_bus, int slave_addr);

int main(int argc, char *argv[]) {
    if (i2c_bus_start(I2C_BUS, PCF8574) == -1) exit(0);

    _FUNCTIONSET 	= FUNCTIONSET_DB5 | FUNCTIONSET_4BITMODE | FUNCTIONSET_2LINE | FUNCTIONSET_5x8DOTS;
    _DISPLAYCTRL 	= DISPLAYCTRL_DB3 | DISPLAYCTRL_DISPLAYON | DISPLAYCTRL_CURSOROFF | DISPLAYCTRL_BLINKOFF;
    _ENTRYMODE 	    = ENTRYMODE_DB2 | ENTRYMODE_INCREMENTS | ENTRYMODE_SHIFTDECREMENT; 
    _CURSORSHIFT 	= CURSORSHIFT_DB4 | CURSORSHIFT_MOVE_LEFT | CURSORSHIFT_DISPLAY_LEFT;

    /*
        Reset function
        On LCD module HD44780U with PCF8574, reset function is initialized by instruction, not Internal Reset Circuit
    */ 
    i2c_write_8bits(0b00110100); // D7=0, D6=0, D5=1, D4=1, BL=0, EN=1, RW=0, RS=0    
    i2c_write_8bits(0b00110000); // D7=0, D6=0, D5=1, D4=1, BL=0, EN=0, RW=0, RS=0
    usleep(1);
    i2c_write_8bits(0b00110100);
    i2c_write_8bits(0b00110000);
    usleep(1);
    i2c_write_8bits(0b00110100);
    i2c_write_8bits(0b00110000); // 8-bit mode init complete
    usleep(1);
    i2c_write_8bits(0b00100100); //
    i2c_write_8bits(0b00100000); // switched now to 4-bit mode

    write_4bits(_FUNCTIONSET, USER_COMMAND);
    write_4bits(_DISPLAYCTRL, USER_COMMAND);
    write_4bits(_ENTRYMODE, USER_COMMAND);
    write_4bits(_CURSORSHIFT, USER_COMMAND);

    write_4bits(LCD_CLEARDISPLAY, USER_COMMAND);
    write_4bits(LCD_RETURNHOME, USER_COMMAND);

    set_cursor(0, 0);
    lcd_write_string("Line 1"); 
    set_cursor(0, 1);
    lcd_write_string("Line 2"); 
    close(i2c_fd);
}	

int i2c_bus_start(const char *i2c_bus, int slave_addr){
    i2c_fd = open(i2c_bus, O_RDWR);
	if (i2c_fd < 0){
        printf("Unable to open I2C bus\n");
        return -1;
	}

    if (ioctl(i2c_fd, I2C_SLAVE, slave_addr) < 0){
        printf("Unable to perform ioctl for PCF8574 as I2C slave\n");
        close(i2c_fd);
        return -1;
    }
    return 1;
}

void i2c_write_8bits(uint8_t data) {
    if (write(i2c_fd, &data, WRITE_SIZE) != 1){
        printf("Unable to set GPIO for PCF8574\n");
        exit(0);
    }

    usleep(1000);// Wait for 1 ms so that 1 byte is written properly
}

void write_4bits(uint8_t value, int func)
{
    // _8_wr_bits is 8 bits to write to LCD
    // _8_wr_bits = RS (bit 8) + EN (bit 6) + BL (bit 5) + 4 bits (D4, D5, D6, D7) 
    uint8_t _8_wr_bits;
    uint8_t data_high = value & 0b11110000; 
    uint8_t data_low = (value << 4) & 0b11110000; 

    if(func == USER_COMMAND) {
        // Write out data_high
        _8_wr_bits = RS_LOW | EN_LOW|BL_ON| data_high;//RS pin LOW, EN LOW, Backlight on (BL_ON)
        i2c_write_8bits(_8_wr_bits);
        usleep(1);
        _8_wr_bits = RS_LOW | EN_HIGH|BL_ON| data_high;//RS pin LOW, EN HIGH to latch out data in next step, Backlight on (BL_ON)
        i2c_write_8bits(_8_wr_bits);
        usleep(1);
        _8_wr_bits = RS_LOW | EN_LOW|BL_ON| data_high;//RS pin LOW, EN LOW to latch out data, Backlight on (BL_ON)
        i2c_write_8bits(_8_wr_bits);
        usleep(1);

        // Write out data_low
        _8_wr_bits = RS_LOW | EN_LOW|BL_ON|data_low;
        i2c_write_8bits(_8_wr_bits);
        usleep(1);
        _8_wr_bits = RS_LOW | EN_HIGH|BL_ON|data_low;
        i2c_write_8bits(_8_wr_bits);
        usleep(1);
        _8_wr_bits = RS_LOW | EN_LOW|BL_ON|data_low;
        i2c_write_8bits(_8_wr_bits);
        usleep(1);
    }
    else if(func == USER_WRITE) {
        _8_wr_bits = RS_HIGH | EN_LOW|BL_ON|data_high;
        i2c_write_8bits(_8_wr_bits);
        usleep(1);
        _8_wr_bits = RS_HIGH | EN_HIGH|BL_ON|data_high;
        i2c_write_8bits(_8_wr_bits);
        usleep(1);
        _8_wr_bits = RS_HIGH | EN_LOW|BL_ON|data_high;
        i2c_write_8bits(_8_wr_bits);
        usleep(1);

        _8_wr_bits = RS_HIGH | EN_LOW|BL_ON|data_low;
        i2c_write_8bits(_8_wr_bits);
        usleep(1);
        _8_wr_bits = RS_HIGH | EN_HIGH|BL_ON|data_low;
        i2c_write_8bits(_8_wr_bits);
        usleep(1);
        _8_wr_bits = RS_HIGH | EN_LOW|BL_ON|data_low;
        i2c_write_8bits(_8_wr_bits);
        usleep(1);
    }
}

void lcd_write_char(char character)
{
  write_4bits(character, USER_WRITE);
}

void lcd_write_string(char *string)
{
  while(*string)lcd_write_char(*string++);
}

void set_cursor(int column, int row)
{
	uint8_t DRAM_ADDRESS = 0x00;
	uint8_t DRAM_OFFSET[2] = {0x00, 0x40};
	if(column >= COLUMNS) column = COLUMNS - 1;
	if(row >= ROWS) row = ROWS - 1;
	DRAM_ADDRESS = DRAM_OFFSET[row] + column;
	
	write_4bits(SETDDRAMADDR_2_LINE|DRAM_ADDRESS, USER_COMMAND);
}