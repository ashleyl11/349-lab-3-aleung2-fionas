#include <unistd.h>

#include <i2c.h>
#include <lcd_driver.h>
#include <printk.h>

#define PCF8574_ADDR 0x27
#define LCD_BACKLIGHT 0x08 //P3
#define LCD_ENABLE 0x04 //P2
#define LCD_RS 0x01 //P0

//creates instuction and send to lcd for initalizing the lcd
void send_lcd_instr(uint8_t num, uint8_t rs){
    uint8_t data[2];
    data[0] = (num << 4) | LCD_BACKLIGHT | LCD_ENABLE | rs;
    data[1] = (num << 4) | LCD_BACKLIGHT | rs;

    i2c_master_write(data, 2, PCF8574_ADDR);
}

void lcd_send_byte(uint8_t byte, uint8_t rs) {
    uint8_t higher = (byte >> 4) & 0x0F; // upper 4 bits
    uint8_t lower = byte & 0x0F; //lower 4 bits

    uint8_t data[4];
    data[0] = (higher << 4) | LCD_BACKLIGHT | LCD_ENABLE | rs; // upper E=1
    data[1] = (higher << 4) | LCD_BACKLIGHT | rs; // upper E=0
    data[2] = (lower << 4) | LCD_BACKLIGHT | LCD_ENABLE | rs;
    data[3] = (lower << 4) | LCD_BACKLIGHT | rs;

    i2c_master_write(data, 4, PCF8574_ADDR);
}
void lcd_driver_init() {
    i2c_master_init(100);
    for (volatile int i = 0; i < 1000000; i++);

    //initializing HD44780U
    //interface is 8 bits long
    send_lcd_instr(0x3, 0);
    for (volatile int i = 0; i < 100000; i++);
    send_lcd_instr(0x3, 0);
    for (volatile int i = 0; i < 100000; i++);
    send_lcd_instr(0x3, 0);
    for (volatile int i = 0; i < 10000; i++);
    //sets interface to 4 bits long
    send_lcd_instr(0x2, 0);
    for (volatile int i = 0; i < 10000; i++);
    
    // clears display
    lcd_send_byte(0x01, 0);
    for (volatile int i = 0; i < 1000000; i++);
    
}

void lcd_print(char *input){
    while (*input) {
        lcd_send_byte(*input, LCD_RS);
        input++;
    }
}

void lcd_set_cursor(uint8_t row, uint8_t col){
    uint8_t addr;

    if (row == 0) {
        addr = 0x00 + col;
    } else {
        addr = 0x40 + col;
    }
    lcd_send_byte(0x80 | addr, 0);
}

void lcd_clear() {
    lcd_send_byte(0x01, 0);
    for (volatile int i = 0; i < 1000000; i++);
}
