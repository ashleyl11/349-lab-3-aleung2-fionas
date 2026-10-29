/**
 * @file kernel.c
 *
 * @brief      Kernel entry point
 *
 * @date  oct 11, 2026
 * @author ashley leung (aleung2) fiona shi (fionas)  
 */

#include <arm.h>
#include <kernel.h>
#include <systick.h>
#include <uart.h>
#include <printk.h>
#include <timer.h>
#include <gpio.h>
#include <servok.h>

/** @brief UART baud rate for the serial console */
#define BAUD_RATE 115200

/** @brief System Handler Priority Register 2; bits [31:24] hold SVCall's priority */
#define SHPR2 ((volatile uint32_t *)0xE000ED1C)

/** @brief Lowest possible priority for SVCall, so interrupts can preempt syscalls */
#define SVC_LOWEST_PRIORITY (0xFFu << 24)


int kernel_main( void ) {
  init_349(); // DO NOT REMOVE THIS LINE


  systick_init();
  uart_init(BAUD_RATE);
  servo_init();

  // let UART, SysTick, and timer interrupts run during system calls
  *SHPR2 = SVC_LOWEST_PRIORITY;

  enter_user_mode();
  return 0;
}
