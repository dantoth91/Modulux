/*
    Smarty - Copyright (C) 2015
    GAMF MegaLux Team
*/

#include "one_vire_comm.h"


#include "ds18b20.h"
#include "chprintf.h"

static WORKING_AREA(wa_ds18b20_polling, 512);

volatile float currentTemp[5];
static int seconds = 1;
static int sensor_num;

static msg_t ds18b20_polling_thread(void *p)
{
    systime_t time;
    
    time = chTimeNow();
    while (true)
    {
        time += MS2ST(500);

        if(sensor_num == 1){
          currentTemp[0] = DS18B20_GetTemp(0);
        }

        else{
          currentTemp[seconds - 1] = DS18B20_GetTemp(seconds);
        }

        seconds ++;
        seconds = (seconds > sensor_num) ? 1 : seconds;

        chThdSleepUntil(time);
    }
}

void one_vireInit(void){

	DS18B20_Init();

    sensor_num = DS18B20_ScanBus();

  /*
   * Creates the dsp18b20 (OneWire bus) thread.
   */
  chThdCreateStatic(wa_ds18b20_polling, sizeof(wa_ds18b20_polling), NORMALPRIO, ds18b20_polling_thread, NULL);

}

uint32_t one_vireGetValue(int num){
  
  if (sensor_num == 1 && num == 1){
    return (uint32_t)(currentTemp[0]*10);
  }

  else if (sensor_num == 1 && num > 0){
    return 1;
  }

  if (num > sensor_num){
    return 2;
  }

  return (uint32_t)(currentTemp[num]*10); 	  
}


void cmd_one_virevalues(BaseSequentialStream *chp, int argc, char *argv[]){

  (void)argc;
  (void)argv;
  chprintf(chp, "\x1B\x63");
  chprintf(chp, "\x1B[2J");
  while (chnGetTimeout((BaseChannel *)chp, TIME_IMMEDIATE) == Q_TIMEOUT) {
      chprintf(chp, "\x1B[%d;%dH", 0, 0);

      chprintf(chp, "SOLAR CELL MODULE TEMPERATURES\r\n");
      chprintf(chp, "Temp0 - MODULE 1 :%10d\r\n", one_vireGetValue(0));
      chprintf(chp, "Temp1 - MODULE 8 :%10d\r\n", one_vireGetValue(1));
      chprintf(chp, "Temp2 - MODULE 6 :%10d\r\n", one_vireGetValue(2));
      chprintf(chp, "Temp3 - MODULE 12:%10d\r\n", one_vireGetValue(3));
      chprintf(chp, "Temp4 - MODULE 2: %10d\r\n", one_vireGetValue(4));

      chThdSleepMilliseconds(1000);
  }
}
