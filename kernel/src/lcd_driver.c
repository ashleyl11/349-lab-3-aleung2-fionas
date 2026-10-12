#include <unistd.h>

#include <i2c.h>
#include <lcd_driver.h>
#include <printk.h>
#include <systick.h>

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
    // wait for more than 15 ms
    systick_delay(16);

    i2c_master_init(100);

    //initializing HD44780U
    //interface is 8 bits long
    send_lcd_instr(0x3, 0);
    systick_delay(5); // wait > 4.1 ms 

    send_lcd_instr(0x3, 0);
    systick_delay(1); // wait > 100 us

    send_lcd_instr(0x3, 0);
    
    //sets interface to 4 bits long
    send_lcd_instr(0x2, 0);
    
    // clears display
    lcd_clear();
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
    systick_delay(2000);
}
