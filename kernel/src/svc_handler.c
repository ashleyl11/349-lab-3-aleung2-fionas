/**
 * @file svc_handler.c
 *
 * @brief  decodes the system call number from the svc instruction    
 *
 * @date  oct 29, 2026 
 *
 * @author ashley leung (aleung2), fiona shi (fionas)    
 */

#include <stdint.h>
#include <debug.h>
#include <syscall.h>
#include <svc_num.h>
#include <servok.h>

/** @brief Registers the hardware saves on the stack when svc runs */
typedef struct {
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r12;
  uint32_t lr;
  uint32_t pc;
  uint32_t xpsr;
} stack_frame_t;

/** @brief Size in bytes of a Thumb svc instruction */
#define SVC_INSTR_SIZE 2
/** @brief The svc number is the lower 8 bits of the instruction */
#define SVC_NUM_MASK 0xFF

void svc_c_handler(stack_frame_t *frame) {
  uint16_t *svc_instr = (uint16_t *)(frame->pc - SVC_INSTR_SIZE);
  int svc_number = *svc_instr & SVC_NUM_MASK;

  switch (svc_number) {
  case SVC_SBRK:
    frame->r0 = (uint32_t)sys_sbrk((int)frame->r0);
    break;
  case SVC_WRITE:
    frame->r0 = sys_write((int)frame->r0, (char *)frame->r1, (int)frame->r2);
    break;
  case SVC_READ:
    frame->r0 = sys_read((int)frame->r0, (char *)frame->r1, (int)frame->r2);
    break;
  case SVC_EXIT:
    sys_exit((int)frame->r0);
    break;
  case SVC_CLOSE:
  case SVC_FSTAT:
  case SVC_LSEEK:
  case SVC_KILL:
    frame->r0 = (uint32_t)-1;
    break;
  case SVC_ISATTY:
    frame->r0 = 1;
    break;
    case SVC_SERVO_ENABLE:
    frame->r0 = sys_servo_enable((uint8_t)frame->r0, (uint8_t)frame->r1);
    break;
  case SVC_SERVO_SET:
    frame->r0 = sys_servo_set((uint8_t)frame->r0, (uint8_t)frame->r1);
    break;
  default:
    DEBUG_PRINT("Not implemented: svc num %d\n", svc_number);
    ASSERT(0);
  }
}