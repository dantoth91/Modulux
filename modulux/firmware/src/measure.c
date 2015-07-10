/*
 * measure.c
 *
 *  Created on: 2015 júl. 9
 *      Author: Richárd
 */




#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "measure.h"

#define ADC_GRP1_BUF_DEPTH      8
int curr_adc;


static int32_t measValue[MEAS_NUM_CH +2];
static adcsample_t samples[MEAS_NUM_CH * ADC_GRP1_BUF_DEPTH];

static const ADCConversionGroup adcgrpcfg = {
  FALSE,
  MEAS_NUM_CH,
  NULL,
  NULL,
  0,                                                                       /* CR1 */
  ADC_CR2_SWSTART,                                                         /* CR2 */
  ADC_SMPR1_SMP_AN10(ADC_SAMPLE_15),
  ADC_SMPR2_SMP_AN4(ADC_SAMPLE_15),
  ADC_SQR1_NUM_CH(MEAS_NUM_CH),
  NULL,
  ADC_SQR3_SQ1_N(ADC_CHANNEL_IN4) | ADC_SQR3_SQ2_N(ADC_CHANNEL_IN10)
};


void measCalc()
{
  int avg, ch, i;
  double temp = 0;

  adcConvert(&ADCD1, &adcgrpcfg, samples, ADC_GRP1_BUF_DEPTH);

  for(ch = 0; ch < MEAS_NUM_CH; ch++) {
    avg = 0;
    for(i = 0; i < ADC_GRP1_BUF_DEPTH; i++) {
      avg += samples[ch + MEAS_NUM_CH * i];
    }
    avg /= ADC_GRP1_BUF_DEPTH;

    switch(ch)
    {
      case MEAS_UBAT:
        avg *=  5545;
        avg /= 12412;
      break;
      case MEAS_SOLAR_CURRENT:
        avg -= NULL_AMPER_ADC;
        curr_adc = avg;
        temp = avg / AMP_PER_ADC;
        avg = temp*1000;
      break;
    }
    chSysLock();
    measValue[ch] = (int16_t)avg;
    chSysUnlock();
  }

}


int16_t measGetValue(enum measChannels ch){
      return measValue[ch];
}

void init_measures()
{
  adcStart(&ADCD1, NULL);
}

void cmd_getAdcValues(BaseSequentialStream *chp, int argc, char *argv[]) {

  (void)argc;
  (void)argv;
  chprintf(chp, "\x1B\x63");
  chprintf(chp, "\x1B[2J");
  while (chnGetTimeout((BaseChannel *)chp, TIME_IMMEDIATE) == Q_TIMEOUT) {
    chprintf(chp, "\x1B\x63");
    chprintf(chp, "\x1B[2J");

    chprintf(chp, "CHANNELS: %d, BUFFER: %d\r\n", MEAS_NUM_CH, ADC_GRP1_BUF_DEPTH);
    chprintf(chp, "U_bat: %d\r\n", measGetValue(MEAS_UBAT));
    chprintf(chp, "Solar Current: %d, %d\r\n", measGetValue(MEAS_SOLAR_CURRENT), curr_adc);

    chThdSleepMilliseconds(1000);
  }



}
