/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "test.h"
#include "ds18b20.h"
#include "can_comm.h"

static SerialConfig uartCfg =
{
115200, // bit rate
0,
0,
0
};

void serial_init(void)
{
  palSetPadMode(GPIOB, 6, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOB, 7, PAL_MODE_ALTERNATE(7));
  sdInit();
  sdStart(&SD1, &uartCfg);
}
static uint8_t txbuf = 0x01;
void serial_send(void)
{
  //txbuf++;
  //sdWrite(&SD6, (uint8_t *) txbuf, 1);
  sdPut(&SD1, txbuf);
}

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
    palSetPad(GPIOE, 2);       /* Orange.  */

    palClearPad(GPIOD, 12);     //LS1
    chThdSleepMilliseconds(100);
    palClearPad(GPIOE, 2);     /* Orange.  */

    palSetPad(GPIOD, 12);     //LS1
    chThdSleepMilliseconds(100);
    serial_send();

  }
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
  palSetPad(GPIOD, GPIOD_X_5V_EN);

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



  serial_init();

  can_commInit();

  //serial_init();

  /*
   * Creates the example thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  chThdCreateStatic(wa_ds18b20_polling, sizeof(wa_ds18b20_polling), NORMALPRIO, ds18b20_polling_thread, NULL);

  while (TRUE) {
    chThdSleepMilliseconds(500);
  }
}
