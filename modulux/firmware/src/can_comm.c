/*
 * can_comm.c

 *
 *  Created on: 2015 márc. 10
 *      Author: Richárd
 */
#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "can_comm.h"
#include "light.h"

enum canState
{
  CAN_SM,
  CAN_ML,
  CAN_RPY,
  CAN_LC,
  CAN_WAIT,
  CAN_NUM_CH
}canstate;

enum canMessage
{
  CAN_JINDEX= 0x01234567,
  CAN_BINDEX= 0x01111111,
  CAN_MSG_NUM
  } canuzenet;

static CANTxFrame txmsg;
static CANRxFrame rxmsg;

static const CANConfig cancfg = {
  CAN_MCR_ABOM,
  CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
  CAN_BTR_TS1(8) | CAN_BTR_BRP(6),
  0,
  NULL
};

struct can_instance {
  CANDriver     *canp;
  uint32_t      led;
};




static WORKING_AREA(wa_index, 128);
static msg_t Index() {
  pwmEnableChannel(&PWMD1, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD1, 10000));
       chThdSleepMilliseconds(400);
       pwmDisableChannel(&PWMD1, 0);
       return 0;
}




static WORKING_AREA(can_rx_wa, 256);
static msg_t can_rx(void *p) {
  EventListener el;
  int i;
  int z;

  (void)p;
  chRegSetThreadName("receiver");
  chEvtRegister(&CAND1.rxfull_event, &el, 0);
  while(!chThdShouldTerminate()) {
    if (chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(100)) == 0)
      continue;
    while (canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == RDY_OK) {

      //villan();
      Light_state(rxmsg.EID);

    }
  }
  chEvtUnregister(&CAND1.rxfull_event, &el);

  return 0;
}



static WORKING_AREA(can_tx_wa, 256);
static msg_t can_tx(void * p) {

  (void)p;
  chRegSetThreadName("transmitter");
  txmsg.IDE = CAN_IDE_EXT;
  txmsg.EID = 0x01234567;
  txmsg.RTR = CAN_RTR_DATA;
  txmsg.DLC = 8;
  txmsg.data32[0] = 0x55AA55AA;
  txmsg.data32[1] = 0x00FF00FF;

  //canTransmit(&CAND1, &txmsg, MS2ST(100));

  while (!chThdShouldTerminate()) {
    canTransmit(&CAND1, CAN_ANY_MAILBOX ,&txmsg, MS2ST(100));
    chThdSleepMilliseconds(100);
  }
  return 0;
}

void can_commInit(void){
  canStart(&CAND1, &cancfg);
   //chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 7, can_tx, NULL);
   chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO, can_rx, NULL);
}





