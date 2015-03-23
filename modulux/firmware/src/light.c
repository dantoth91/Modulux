/*
    Smarty - Copyright (C) 2015
    GAMF MegaLux Team
*/

#include "ch.h"
#include "hal.h"

#include "light.h"
#include "can_comm.h"
#include "chprintf.h"





int indexEnable( ){

indexTrue=1;

}
int indexDisable( ){

indexTrue=2;

}



static PWMConfig pwmcfg = {
  1000000,  /* 1MHz PWM clock frequency */
  1000,    /* PWM period 1 milliseconds */
  NULL,
  {
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
};

static uint32_t ido;
int seg=1;
int seg2=1;
static int tim_db=0;
void lightCalc(void){

  tim_db++;
 //  if(tim_db>10)
 //  {
  //   pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 100));
  // }
  switch (allapot) {
  case LIGHT_JOBBINDEX:     {     palTogglePad(GPIOE,2);
 }
  case LIGHT_BALINDEX:   { }
  }

  /*


  if(indexTrue==1){
    palTogglePad(GPIOE,2);
    pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 10000));
    indexTrue=0;
    tim_db=0;
  }

  if(indexTrue==2){
     palTogglePad(GPIOE,2);
     pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));
     indexTrue=0;
     tim_db=0;
   }
*/

}

void lightInit(void){
    pwmStart(&PWMD1, &pwmcfg);

}
