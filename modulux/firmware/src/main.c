/*
    Modulux - Copyright (C) 2014
    GAMF MegaLux Team              
*/


#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "test.h"


/*===========================================================================*/
/* Generic code.                                                             */
/*===========================================================================*/

/*
 * Red LED blinker thread, times are in milliseconds.
 */
static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

  systime_t time;

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
    time = 500;
    palClearPad(GPIOE, GPIOE_LD1);
    palClearPad(GPIOB, GPIOB_TXD1);
    palClearPad(GPIOB, GPIOB_RXD1);
    chThdSleepMilliseconds(time);
    palSetPad(GPIOE, GPIOE_LD1);
    palSetPad(GPIOB, GPIOB_TXD1);
    palSetPad(GPIOB, GPIOB_RXD1);
    chThdSleepMilliseconds(time);
  }
  return 0; /* Never executed.*/
}
/*
 * 20ms Task
 */
static WORKING_AREA(watask20ms, 256);
static msg_t task20ms(void *arg) {
  systime_t time; 

  (void)arg;
  chRegSetThreadName("task20ms");
  time = chTimeNow();  
  while (TRUE) {
    time += MS2ST(20);



    chThdSleepUntil(time);
  }
  return 0; /* Never executed.*/
}
/*
 * Application entry point.
 */
int main(void) {

  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /* 5V Enabled */
  palSetPad(GPIOD, 4);

  /*
   * Creates the 20ms Task.
   */
  chThdCreateStatic(watask20ms, sizeof(watask20ms), NORMALPRIO, task20ms, NULL);
  
  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (TRUE) {
    chThdSleepMilliseconds(1000);
  }
}
