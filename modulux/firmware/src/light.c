/*
    Smarty - Copyright (C) 2015
    GAMF MegaLux Team
*/

#include "ch.h"
#include "hal.h"

#include "light.h"
#include "can_comm.h"
#include "chprintf.h"

#define LIGHTS_PERIOD   15

static enum canMessages
{
  LIGHT_RIGHT,
  LIGHT_LEFT,
  LIGHT_WARNING,
  LIGHT_ACTIVE,
  LIGHT_WAIT
}lightState;

static PWMConfig pwmcfg = {
  1000000,  /* 1MHz PWM clock frequency */
  1000,    /* PWM period 1 milliseconds */
  NULL,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
};

static int period_num=0;

void lightCalc(void){

  switch (lightState) {
    case LIGHT_RIGHT:
      pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));
      period_num=0;
      lightState=LIGHT_ACTIVE;
      break;

    case LIGHT_LEFT:
      pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));
      period_num=0;
      lightState=LIGHT_ACTIVE;
      break;

    case LIGHT_WARNING:
      pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));
      pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 1000));
      period_num=0;
      lightState=LIGHT_ACTIVE;
      break;

    case LIGHT_ACTIVE:
      period_num++;
      if(period_num > LIGHTS_PERIOD){
        palTogglePad(GPIOE,GPIOE_LD2);
        pwmDisableChannel(&PWMD1, 2);
        pwmDisableChannel(&PWMD1, 1);
        period_num=0;
        lightState = LIGHT_WAIT;
      }
      break;
    case LIGHT_WAIT:
      break;
    default :
      break;
  }
}

void lightRight(void){
if(lightState == LIGHT_WAIT){
  lightState = LIGHT_RIGHT;
}
}

void lightLeft(void){
if(lightState == LIGHT_WAIT){
  lightState = LIGHT_LEFT;
}
}

void lightWarning(void){
if(lightState == LIGHT_WAIT){
  lightState = LIGHT_WARNING;
}
}

void lightInit(void){
    pwmStart(&PWMD1, &pwmcfg);
    lightState = LIGHT_WAIT;
    period_num = 0;
}
