/*
    Modulux - Copyright (C) 2014
    GAMF MegaLux Team              
*/

#include "ch.h"
#include "hal.h"
#include "test.h"

#include "console.h"
#include "ds18b20.h"
#include "can_comm.h"
#include "light.h"

static WORKING_AREA(wa_ds18b20_polling, 512);

volatile float currentTemp;
static msg_t ds18b20_polling_thread(void *p)
{
    chThdSleep(1000);
    DS18B20_Init();

    int seconds = 0;
    systime_t time = chTimeNow();
    while (true)
    {
        time += MS2ST(500);

        currentTemp = DS18B20_GetTemp(0);

        chThdSleepUntil(time);
    }
}



static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {
    palTogglePad(GPIOE, GPIOE_LD1);
    chThdSleepMilliseconds(100);
  }
  return 0; /* Never executed.*/
}


static WORKING_AREA(watask20ms, 256);
static msg_t task20ms(void *arg) {
  systime_t time;

  (void)arg;
  chRegSetThreadName("task20ms");
  time = chTimeNow();
  while (TRUE) {
    time += MS2ST(20);

    lightCalc();



    chThdSleepUntil(time);
  }
  return 0; /* Never executed.*/
}
/*
 * Application entry point.
 */
int main(void) {

  halInit();
  chSysInit();

  palClearPad(GPIOE, GPIOE_PO1);     //PO1
  palClearPad(GPIOE, GPIOE_PO2);     //PO2
  palClearPad(GPIOE, GPIOE_PO3);     //PO3
  palClearPad(GPIOE, GPIOE_PO4);     //PO4
  palClearPad(GPIOA, GPIOA_PO5);     //PO5
  palClearPad(GPIOA, GPIOA_PO6);     //PO6

  palSetPad(GPIOD, GPIOD_LS_1);     //LS1
  palSetPad(GPIOD, GPIOD_LS_2);     //LS2
  palSetPad(GPIOD, GPIOD_LS_3);     //LS3
  palSetPad(GPIOD, GPIOD_LS_4);     //LS4

  /*
   * 5V Enable
   */
  palSetPad(GPIOD, GPIOD_X_5V_EN);

  /*
   * Shell manager initialization.
   */
  consoleInit();

  /*
   * CAN bus initialization.
   */
  can_commInit();

  /*
   * Light control initialization.
   */
  lightInit();

  /*
   * Creates the 20ms Task.
   */
  chThdCreateStatic(watask20ms, sizeof(watask20ms), NORMALPRIO, task20ms, NULL);

  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  
  /*
   * Creates the dsp18b20 (OneWire bus) thread.
   */
  chThdCreateStatic(wa_ds18b20_polling, sizeof(wa_ds18b20_polling), NORMALPRIO, ds18b20_polling_thread, NULL);

  while (TRUE) {
    consoleStart();
    chThdSleepMilliseconds(1000);
  }
}
