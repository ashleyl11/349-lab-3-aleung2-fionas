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

int kernel_main( void ) {
  init_349(); // DO NOT REMOVE THIS LINE

  systick_init();

  return 0;
}
