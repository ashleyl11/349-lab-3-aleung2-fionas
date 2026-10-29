#include <stdio.h>
#include <servo.h>
#include <unistd.h>
#include <string.h>


/** @brief File descriptor for standard input */
#define STDIN_FD 0
/** @brief Max length of one typed command line */
#define LINE_LEN 64
/** @brief Max length of the command word */
#define CMD_LEN 16
/** @brief Lowest channel number the user types */
#define MIN_CHANNEL 1
/** @brief Highest channel number the user types */
#define MAX_CHANNEL 2
/** @brief Largest valid servo angle */
#define MAX_ANGLE 180


static void print_help(void) {
  printf("Welcome to Servo Controller!\n");
  printf("Commands\n");
  printf("enable <ch>: Enable servo channel\n");
  printf("disable <ch>: Disable servo channel\n");
  printf("set <ch> <angle>: Set servo angle\n");
}
static int run_command(const char *line) {
  char cmd[CMD_LEN];
  int channel;
  int angle;
  char extra;

  int fields = sscanf(line, "%15s %d %d %c", cmd, &channel, &angle, &extra);

  if (fields < 2 || channel < MIN_CHANNEL || channel > MAX_CHANNEL) {
    return -1;
  }
  if (strcmp(cmd, "enable") == 0 && fields == 2) {
    return servo_enable(channel - MIN_CHANNEL, 1);
  }
  if (strcmp(cmd, "disable") == 0 && fields == 2) {
    return servo_enable(channel - MIN_CHANNEL, 0);
  }
  if (strcmp(cmd, "set") == 0 && fields == 3
      && angle >= 0 && angle <= MAX_ANGLE) {
    return servo_set(channel - MIN_CHANNEL, angle);
  }
  return -1;
}

int main(void) {
  char line[LINE_LEN];

  print_help();

  while (1) {
    printf("> ");
    fflush(stdout);

    int len = read(STDIN_FD, line, LINE_LEN - 1);
    if (len <= 0) {
      break;
    }
    line[len] = '\0';

    if (len == 1 && line[0] == '\n') {
      continue;
    }

    if (run_command(line) != 0) {
      printf("Invalid command\n");
    }
  }
  return 0;
}