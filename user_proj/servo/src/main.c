#include <stdio.h>

#define UNUSED __attribute__((unused))

#include <servo.h>
#include <unistd.h>

// int main(UNUSED int argc, UNUSED char const *argv[]){
//   printf("Hello World from User.\n");
//   return 0;
// }

int main(void) {
  fwrite("hello from user mode\n", 1, 21, stdout);
  fflush(stdout);
  return 3;
}