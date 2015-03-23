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
int indexEnable();


static int indexTrue= 0 ;

static enum canMessages
{
  LIGHT_JOBBINDEX,
  LIGHT_BALINDEX,
  LIGHT_FEK,
  LIGHT_TOLATO
}allapot;

void cmd_lightvalues(BaseSequentialStream *chp, int argc, char *argv[]);
#endif
