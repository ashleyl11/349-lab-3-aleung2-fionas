#include <unistd.h>

#include <gpio.h>
#include <keypad_driver.h>

#define KEYPAD_COL1_PORT GPIO_B
#define KEYPAD_COL1_PIN  6
#define KEYPAD_COL2_PORT GPIO_A
#define KEYPAD_COL2_PIN  9
#define KEYPAD_COL3_PORT GPIO_A
#define KEYPAD_COL3_PIN  6

#define KEYPAD_ROW1_PORT GPIO_C
#define KEYPAD_ROW1_PIN  7
#define KEYPAD_ROW2_PORT GPIO_A
#define KEYPAD_ROW2_PIN  0
#define KEYPAD_ROW3_PORT GPIO_A
#define KEYPAD_ROW3_PIN  5
#define KEYPAD_ROW4_PORT GPIO_A
#define KEYPAD_ROW4_PIN  7

void keypad_init() {
    gpio_init(KEYPAD_COL1_PORT, KEYPAD_COL1_PIN, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);
    gpio_init(KEYPAD_COL2_PORT, KEYPAD_COL2_PIN, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);
    gpio_init(KEYPAD_COL3_PORT, KEYPAD_COL3_PIN, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);
    gpio_init(KEYPAD_ROW1_PORT, KEYPAD_ROW1_PIN, MODE_INPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_UP, ALT0);
    gpio_init(KEYPAD_ROW2_PORT, KEYPAD_ROW2_PIN, MODE_INPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_UP, ALT0);
    gpio_init(KEYPAD_ROW3_PORT, KEYPAD_ROW3_PIN, MODE_INPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_UP, ALT0);
    gpio_init(KEYPAD_ROW4_PORT, KEYPAD_ROW4_PIN, MODE_INPUT, OUTPUT_PUSH_PULL, OUTPUT_SPEED_LOW, PUPD_PULL_UP, ALT0);
}

void set_col(int col){
    //set all col high first
    gpio_set(KEYPAD_COL1_PORT, KEYPAD_COL1_PIN);
    gpio_set(KEYPAD_COL2_PORT, KEYPAD_COL2_PIN);
    gpio_set(KEYPAD_COL3_PORT, KEYPAD_COL3_PIN);

    // sets given col low
    if (col == 1) {
        gpio_clr(KEYPAD_COL1_PORT, KEYPAD_COL1_PIN);
    } else if (col == 2) {
        gpio_clr(KEYPAD_COL2_PORT, KEYPAD_COL2_PIN);
    } else if (col == 3) {
        gpio_clr(KEYPAD_COL3_PORT, KEYPAD_COL3_PIN);
    }
}

int read_row(int row){
    if (row == 1) {
        return gpio_read(KEYPAD_ROW1_PORT, KEYPAD_ROW1_PIN);
    } else if (row == 2) {
        return gpio_read(KEYPAD_ROW2_PORT, KEYPAD_ROW2_PIN);
    } else if (row == 3) {
        return gpio_read(KEYPAD_ROW3_PORT, KEYPAD_ROW3_PIN);
    } else if (row == 4) {
        return gpio_read(KEYPAD_ROW4_PORT, KEYPAD_ROW4_PIN);
    }
    return 1;
}

char keypad_read() {
    static char prev_key = '\0';
    char curr_key = '\0';
    //sets col 1 to low first and reads every row to see if it is set low
    // if it is set low that means the button is pressed
    set_col(1);
    for (volatile int i = 0; i < 100000; i++);
    if (read_row(1) == 0) curr_key = '1';
    if (read_row(2) == 0) curr_key = '4';
    if (read_row(3) == 0) curr_key = '7';
    if (read_row(4) == 0) curr_key = '*';

    set_col(2);
    for (volatile int i = 0; i < 100000; i++);
    if (read_row(1) == 0) curr_key = '2';
    if (read_row(2) == 0) curr_key = '5';
    if (read_row(3) == 0) curr_key = '8';
    if (read_row(4) == 0) curr_key = '0';

    set_col(3);
    for (volatile int i = 0; i < 100000; i++);
    if (read_row(1) == 0) curr_key = '3';
    if (read_row(2) == 0) curr_key = '6';
    if (read_row(3) == 0) curr_key = '9';
    if (read_row(4) == 0) curr_key = '#';

    // only returns the key if it wasn't already being pressed
    if (curr_key != '\0' && prev_key == '\0') {
        prev_key = curr_key;
        for (volatile int i = 0; i < 100000; i++);
        return curr_key;
    }
    prev_key = curr_key;

    return '\0';
}
