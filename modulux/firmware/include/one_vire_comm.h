/*
    Smarty - Copyright (C) 2015
    GAMF MegaLux Team
*/


#ifndef ONE_VIRE_INCLUDED
#define ONE_VIRE_H_INCLUDED

#include "ch.h"
#include "hal.h"

void one_vireInit(void);
uint32_t one_vireGetValue(int num);

void cmd_one_virevalues(BaseSequentialStream *chp, int argc, char *argv[]);
#endif
