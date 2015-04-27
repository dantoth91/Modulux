/*
    Modulux - Copyright (C) 2014
    GAMF MegaLux Team              
*/

#include <string.h>
#include <stdlib.h>

#include "ch.h"
#include "hal.h"

#include "test.h"
#include "shell.h"
#include "chprintf.h"

#include "console.h"
#include "can_comm.h"
#include "one_vire_comm.h"
#include "ds18b20.h"
#include "light.h"

/* libc stub */
int _getpid(void) {return 1;}
/* libc stub */
void _exit(int i) {(void)i;}
/* libc stub */
#include <errno.h>
#undef errno
extern int errno;
int _kill(int pid, int sig) {
  (void)pid;
  (void)sig;
  errno = EINVAL;
  return -1;
}

#define SHELL_WA_SIZE   THD_WA_SIZE(2304)
#define TEST_WA_SIZE    THD_WA_SIZE(256)

static SerialConfig ser_cfg = {
  115200,
  0,
  0,
  0,
};

void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]);
void cmd_test(BaseSequentialStream *chp, int argc, char *argv[]);

Thread *consoleThread;

static const ShellCommand commands[] = {
  {"mem", cmd_mem},
  {"threads", cmd_threads},
  {"test", cmd_test},
  {"canvalues", cmd_can_commvalues},
  {"one_vire", cmd_one_virevalues},
  {"ds18b20", cmd_ds18b20_values},
  {"light", cmd_lightblink},
  {NULL, NULL}
};


static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream  *)&SD1,
  commands
};

void consoleInit(void){
  /* Shell initialization.*/

  sdStart(&SD1, &ser_cfg);
  shellInit();
  consoleThread = NULL;
}

void consoleStart(void){
  if (!consoleThread) {
    consoleThread = shellCreate(&shell_cfg1, SHELL_WA_SIZE, NORMALPRIO);
  }
  else if (chThdTerminated(consoleThread)) {
    chThdRelease(consoleThread);    /* Recovers memory of the previous shell.   */
    consoleThread = NULL;           /* Triggers spawning of a new shell.        */
  }
}

void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]) {
  size_t n, size;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: mem\r\n");
    return;
  }
  n = chHeapStatus(NULL, &size);
  chprintf(chp, "core free memory : %u bytes\r\n", chCoreStatus());
  chprintf(chp, "heap fragments   : %u\r\n", n);
  chprintf(chp, "heap free total  : %u bytes\r\n", size);
}

void cmd_threads(BaseSequentialStream *chp, int argc, char *argv[]) {
  static const char *states[] = {THD_STATE_NAMES};
  Thread *tp;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: threads\r\n");
    return;
  }
  chprintf(chp, "    addr    stack prio refs     state time\r\n");
  tp = chRegFirstThread();
  do {
    chprintf(chp, "%.8lx %.8lx %4lu %4lu %9s %lu\r\n",
            (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
            (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
            states[tp->p_state], (uint32_t)tp->p_time);
    tp = chRegNextThread(tp);
  } while (tp != NULL);
}

void cmd_test(BaseSequentialStream *chp, int argc, char *argv[]) {
  Thread *tp;

  (void)argv;
  if (argc > 0) {
    chprintf(chp, "Usage: test\r\n");
    return;
  }
  tp = chThdCreateFromHeap(NULL, TEST_WA_SIZE, chThdGetPriority(),
                           TestThread, chp);
  if (tp == NULL) {
    chprintf(chp, "out of memory\r\n");
    return;
  }
  chThdWait(tp);
}