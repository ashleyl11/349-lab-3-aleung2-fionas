/**
* @file systick.c
*
* @brief SysTick control functions
*
* @date  11 Oct 2026
*
* @author ashley leung (aleung2), fiona shi (fionas)
*/

#include <unistd.h>
#include <systick.h>

/** @brief The SysTick Timer register map. */
struct systick_reg_map {
    volatile uint32_t STK_CTRL;
    volatile uint32_t STK_LOAD;
    volatile uint32_t STK_VAL;
    volatile uint32_t STK_CALIB;
};

#define SYSTICK_BASE (struct systick_reg_map*) 0xE000E010

#define UNUSED __attribute__((unused))

volatile uint32_t systicks = 0;

void systick_init() {
    struct systick_reg_map *systick = SYSTICK_BASE;

    // setting the reload value
    // per 1 ms -> base freq (hz) / 1000
    systick->STK_LOAD = (BASE_FREQ / 1000) - 1;

    // set current value to 0
    systick->STK_VAL = 0;

    // enable counter, int, set clksrc to proc clock
    systick->STK_CTRL |= 7;
}

void systick_delay(UNUSED uint32_t ticks) {
    uint32_t start_tick = systick_get_ticks();
    while ((systick_get_ticks() - start_tick) < ticks){
        continue;
    }
}

uint32_t systick_get_ticks() {
    return systicks;
}

void systick_c_handler() {
    systicks++;
}