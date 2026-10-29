/**
 * @file servok.c
 *
 * @brief Servo system calls
 *
 * @date oct 29, 2026   
 *
 * @author ashley leung (aleung2), fiona shi (fionas) 
 */

#include <unistd.h>
#include <syscall.h>
#include <servok.h>
#include <gpio.h>
#include <timer.h>

/** @brief Timer that drives the servo pulses */
#define SERVO_TIMER 2
/** @brief Divide 16 MHz down to 1 MHz, so timer counts are 1 us */
#define SERVO_PRESCALAR 16
/** @brief Interrupt every 10 us */
#define SERVO_TICK_US 10
/** @brief Ticks per 20 ms frame */
#define SERVO_FRAME_TICKS 2000
/** @brief Pulse length for 0 degrees: 0.6 ms = 60 ticks */
#define SERVO_MIN_TICKS 60
/** @brief Largest allowed angle */
#define SERVO_MAX_ANGLE 180

#define PWM0_PORT GPIO_B
#define PWM0_PIN 3
#define PWM1_PORT GPIO_A
#define PWM1_PIN 4

/** @brief Pulse length for channel 0, in ticks */
int pwm_channel0 = SERVO_MIN_TICKS + 90;
/** @brief Pulse length for channel 1, in ticks*/
int pwm_channel1 = SERVO_MIN_TICKS + 90;
/** @brief Whether channel 0 is sending pulses */
int ch0_en = 0;
/** @brief Whether channel 1 is sending pulses */
int ch1_en = 0;

void servo_init(void){
  gpio_init(PWM0_PORT, PWM0_PIN, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL,
            OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);
  gpio_init(PWM1_PORT, PWM1_PIN, MODE_GP_OUTPUT, OUTPUT_PUSH_PULL,
            OUTPUT_SPEED_LOW, PUPD_NONE, ALT0);
  timer_init(SERVO_TIMER, SERVO_PRESCALAR, SERVO_TICK_US);
}

void servo_irq_handler(void){
  static int tick = 0;

  if (tick == 0) {
    if (ch0_en) {
      gpio_set(PWM0_PORT, PWM0_PIN);
    }
    if (ch1_en) {
      gpio_set(PWM1_PORT, PWM1_PIN);
    }
  }
  if (tick == pwm_channel0) {
    gpio_clr(PWM0_PORT, PWM0_PIN);
  }
  if (tick == pwm_channel1) {
    gpio_clr(PWM1_PORT, PWM1_PIN);
  }

  tick++;
  if (tick == SERVO_FRAME_TICKS) {
    tick = 0;
  }

  timer_clear_interrupt_bit(SERVO_TIMER);
}

int sys_servo_enable(uint8_t channel, uint8_t enabled){
  if (channel == 0) {
    ch0_en = enabled;
  } else if (channel == 1) {
    ch1_en = enabled;
  } else {
    return -1;
  }
  return 0;
}

int sys_servo_set(uint8_t channel, uint8_t angle){
  if (angle > SERVO_MAX_ANGLE) {
    return -1;
  }

  if (channel == 0) {
    pwm_channel0 = SERVO_MIN_TICKS + angle;
  } else if (channel == 1) {
    pwm_channel1 = SERVO_MIN_TICKS + angle;
  } else {
    return -1;
  }
  return 0;
}