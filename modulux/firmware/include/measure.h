/*
 * measure.h
 *
 *  Created on: 2015 júl. 9
 *      Author: Richárd
 */

#ifndef INCLUDE_MEASURE_H_
#define INCLUDE_MEASURE_H_

enum measChannels
{
  MEAS_UBAT,
  MEAS_SOLAR_CURRENT,
  MEAS_NUM_CH
};

#define NULL_AMPER_ADC   1370
#define AMP_PER_ADC      10.814

void init_measures();
void cmd_getAdcValues(BaseSequentialStream *chp, int argc, char *argv[]);
int16_t measGetValue(enum measChannels ch);


#endif /* INCLUDE_MEASURE_H_ */
