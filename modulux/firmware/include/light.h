/*
    Smarty - Copyright (C) 2015
    GAMF MegaLux Team
*/


#ifndef LIGHT_H_INCLUDED
#define LIGHT_H_INCLUDED

#include <math.h>
#include "ch.h"
#include "hal.h"

void lightInit(void);
void lightCalc(void);

void lightRight(void);
void lightLeft(void);
void lightWarning(void);

uint8_t lightBrakeON(void);
uint8_t lightBrakeOFF(void);
uint8_t lightPosLampON(void);
uint8_t lightPosLampOFF(void);

void cmd_lightblink(BaseSequentialStream *chp, int argc, char *argv[]);
#endif
