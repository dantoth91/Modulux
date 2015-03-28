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

void cmd_lightvalues(BaseSequentialStream *chp, int argc, char *argv[]);
#endif
