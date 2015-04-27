/*
    Smarty - Copyright (C) 2015
    GAMF MegaLux Team
*/

#include "ch.h"
#include "hal.h"

#include "light.h"
#include "can_comm.h"
#include "chprintf.h"

#define LIGHTS_PERIOD           30
#define REAR_LIGHTS_INTENSITY   500

static struct lightChanels 
{
  bool lights_disabled;
  bool right;
  bool left;
  bool warning;
  bool braking;
  bool pos_lamp;
} lightchanels;

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

bool_t lightsOn;
bool_t brakeOn;

static uint64_t period;

void lightInit(void){

  period = 0;
  pwmStart(&PWMD1, &pwmcfg);
  period_num = 0;
  lightsOn = FALSE;
  brakeOn = FALSE;
}

void lightCalc(void){

  period++;

  if(period > LIGHTS_PERIOD)
  {
    pwmDisableChannel(&PWMD1, 1);
    pwmDisableChannel(&PWMD1, 2);
  }

  else{
    if(lightchanels.right)
    {
      pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 10000));
      lightchanels.right = FALSE;
    }

    if(lightchanels.left)
    {
      pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 10000));
      lightchanels.left = FALSE;
    }

    if(lightchanels.warning)
    {
      pwmEnableChannel(&PWMD1, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 10000));
      pwmEnableChannel(&PWMD1, 2, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 10000));
      lightchanels.warning = FALSE;
    }
  }

  if(brakeOn){
    pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 10000));
  }
  else{
    if (lightsOn){
      pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, REAR_LIGHTS_INTENSITY));
    }
    else
      pwmDisableChannel(&PWMD1, 0);
  }
}

void lightRight(void){
  lightchanels.lights_disabled = TRUE;
  lightchanels.right = TRUE;
  lightchanels.left = FALSE;
  lightchanels.warning = FALSE;
  period = 0;
}

void lightLeft(void){
  lightchanels.lights_disabled = TRUE;
  lightchanels.right = FALSE;
  lightchanels.left = TRUE;
  lightchanels.warning = FALSE;
  period = 0;
}

void lightWarning(void){
  lightchanels.lights_disabled = TRUE;
  lightchanels.right = FALSE;
  lightchanels.left = FALSE;
  lightchanels.warning = TRUE;
  period = 0;
}

uint8_t lightBrakeON(void){
  brakeOn = TRUE;
  return 1;
}
uint8_t lightBrakeOFF(void){
  brakeOn = FALSE;
  return 1;
}

uint8_t lightPosLampON(void){
  lightsOn = TRUE;
  return 1;
}
uint8_t lightPosLampOFF(void){
  lightsOn = FALSE; 
  return 1;
}

void cmd_lightblink(BaseSequentialStream *chp, int argc, char *argv[]) {

  (void)argc;
  (void)argv;
  chprintf(chp, "\x1B\x63");
  chprintf(chp, "\x1B[2J");
  while (chnGetTimeout((BaseChannel *)chp, TIME_IMMEDIATE) == Q_TIMEOUT) {
    chprintf(chp, "\x1B[%d;%dH", 0, 0);
    if ((argc == 1) && (strcmp(argv[0], "right") == 0)){

      chprintf(chp,"-------------- Blinking right enabled --------------\r\n");
      lightRight();
      return;
    }

    else if ((argc == 1) && (strcmp(argv[0], "left") == 0)){

      chprintf(chp,"-------------- Blinking left enabled --------------\r\n");
      lightLeft();
      return;
    }

    else if ((argc == 1) && (strcmp(argv[0], "warning") == 0)){

      chprintf(chp,"-------------- Blinking warning enabled --------------\r\n");
      lightWarning();
      return;
    }

    else if ((argc == 1) && (strcmp(argv[0], "brakeon") == 0)){

      chprintf(chp,"-------------- Brake light enabled --------------\r\n");
      lightBrakeON();
      return;
    }

    else if ((argc == 1) && (strcmp(argv[0], "brakeoff") == 0)){

      chprintf(chp,"-------------- Brake light disabled --------------\r\n");
      lightBrakeOFF();
      return;
    }

    else if ((argc == 1) && (strcmp(argv[0], "lampon") == 0)){

      chprintf(chp,"-------------- Position lamp enabled --------------\r\n");
      lightPosLampON();
      return;
    }

    else if ((argc == 1) && (strcmp(argv[0], "lampoff") == 0)){

      chprintf(chp,"-------------- Position lamp disabled --------------\r\n");
      lightPosLampOFF();
      return;
    }

    else{
      goto ERROR;
    }
  }

ERROR:
  chprintf(chp, "Usage: light right\r\n");
  chprintf(chp, "       light left\r\n");
  chprintf(chp, "       light warning\r\n");
  chprintf(chp, "       light brakeon\r\n");
  chprintf(chp, "       light brakeoff\r\n");
  chprintf(chp, "       light lampon\r\n");
  chprintf(chp, "       light lampoff\r\n");
  return;
}