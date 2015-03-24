/*
    Smarty - Copyright (C) 2015
    GAMF MegaLux Team
*/

#include "ch.h"
#include "hal.h"

#include "light.h"
#include "can_comm.h"
#include "chprintf.h"



static enum canMessages
{
  LIGHT_JOBBINDEX=0x01234567,
  LIGHT_BALINDEX=0x01111111,
  LIGHT_FEK,
  LIGHT_TOLATO,
  LIGHT_WAIT
}allapot;



static uint32_t can_msg;
void Light_state(uint32_t can_Id){
  can_msg=can_Id;

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





  switch (can_msg) {
    case LIGHT_JOBBINDEX:
        palTogglePad(GPIOE,2);
        pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 10000));
        tim_db=0;
        can_msg=LIGHT_WAIT;
        break;

    case LIGHT_BALINDEX:
      palTogglePad(GPIOE,2);
      pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));
      tim_db=0;
      can_msg=LIGHT_WAIT;
      break;
    case LIGHT_WAIT:
      tim_db++;
      if(tim_db>10){
        pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 100));
        tim_db=0;
      }
      break;
    default :
      break;//
    }


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



void lightInit(void){
    pwmStart(&PWMD1, &pwmcfg);
    allapot=LIGHT_WAIT;

}
