/**
 * @file syscall.c
 *
 * @brief kernel system calls for newlib
 *
 * @date oct 29, 2026      
 *
 * @author ashley leung (aleung2), fiona shi (fionas)
 */

#include <unistd.h>
#include <syscall.h>
#include <uart.h>
#include <lcd_driver.h>
#include <printk.h>
#include <arm.h>

/** @brief File descriptor for standard input */
#define STDIN_FD 0

/** @brief File descriptor for standard output */
#define STDOUT_FD 1

/** @brief Max chars for an int as a string: "-2147483648" plus '\0' */
#define INT_STR_LEN 12

/** @brief Number base for decimal digits */
#define DECIMAL 10

/** @brief Start of the user heap */
extern char __heap_low;
/** @brief End of the user heap */
extern char __heap_top;

/** @brief Current end of the allocated part of the user heap */
static char *heap_break = &__heap_low;

/** @brief ASCII end-of-transmission (Ctrl-D) */
#define EOT 4
/** @brief ASCII delete */
#define DEL 127

void *sys_sbrk(int incr){
  char *prev_break = heap_break;

  if (heap_break + incr > &__heap_top || heap_break + incr < &__heap_low) {
    return (void *)-1;
  }

  heap_break += incr;
  return prev_break;
}

int sys_write(int file, char *ptr, int len){
  if (file != STDOUT_FD || len < 0){
    return -1;
  }
  int bytes_written = 0;
  while (bytes_written < len && uart_put_byte(ptr[bytes_written]) == 0){
    bytes_written++;
  }
  return bytes_written;
}

static char get_char_blocking(void){
  char c;
  while (uart_get_byte(&c) != 0) {
    continue;
  }
  return c;
}

static void put_char_blocking(char c){
  while (uart_put_byte(c) != 0) {
    continue;
  }
}

int sys_read(int file, char *ptr, int len){
  int count = 0;
  if (file != STDIN_FD || len < 0) {
    return -1;
  }

  while (count < len) {
    char c = get_char_blocking();
    if (c == EOT) {
      return count;
    }
    if (c == '\n') {
      continue;
    }
    if (c == '\b' || c == DEL) {
      if (count > 0) {
        count--;
        put_char_blocking('\b');
        put_char_blocking(' ');
        put_char_blocking('\b');
      }
      continue;
    }
    if (c == '\r') {
      c = '\n';
    }
    ptr[count++] = c;
    put_char_blocking(c);
    if (c == '\n') {
      return count;
    }
  }
  return count;
}

static void int_to_str(int num, char *buf){
  char digits[INT_STR_LEN];
  int count = 0;
  int i = 0;

  if (num == 0) {
    buf[0] = '0';
    buf[1] = '\0';
    return;
  }

  if (num < 0) {
    buf[i++] = '-';
    num = -num;
  }

  // collect digits backwards
  while (num > 0) {
    digits[count++] = '0' + (num % DECIMAL);
    num /= DECIMAL;
  }

  // copy digits into buf in the right order
  while (count > 0) {
    buf[i++] = digits[--count];
  }
  buf[i] = '\0';
}

void sys_exit(int status){
  char status_str[INT_STR_LEN];

  // empty the TX buffer first
  uart_flush();
  printk("Exited with status %d\n", status);
  uart_flush();

  int_to_str(status, status_str);
  lcd_clear();
  lcd_print(status_str);

  disable_interrupts();
  while (1) {
    wait_for_interrupt();
  }
}