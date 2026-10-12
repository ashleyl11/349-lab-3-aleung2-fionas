#include <unistd.h>

#include <gpio.h>
#include <keypad_driver.h>
#include <systick.h>

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

char keypad_scan() { // helper
    //sets col 1 to low first and reads every row to see if it is set low
    // if it is set low that means the button is pressed
    set_col(1);
    systick_delay(1);
    if (read_row(1) == 0) return '1';
    if (read_row(2) == 0) return '4';
    if (read_row(3) == 0) return '7';
    if (read_row(4) == 0) return '*';

    set_col(2);
    systick_delay(1);
    if (read_row(1) == 0) return '2';
    if (read_row(2) == 0) return '5';
    if (read_row(3) == 0) return '8';
    if (read_row(4) == 0) return '0';

    set_col(3);
    systick_delay(1);
    if (read_row(1) == 0) return '3';
    if (read_row(2) == 0) return '6';
    if (read_row(3) == 0) return '9';
    if (read_row(4) == 0) return '#';

    return '\0';
}


char keypad_read() {
    static char prev_key = '\0';
    char first_read = keypad_scan();
    systick_delay(10);
    char second_read = keypad_scan();

    // debouncing logic

    // only returns the key if it wasn't already being pressed
    if (first_read != '\0' && prev_key == '\0') {
        if (first_read == second_read){
            prev_key = first_read;
            return first_read;
        }
    }
    prev_key = second_read;

    return '\0';
}
