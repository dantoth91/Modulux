/*
 * can_comm.c

 *
 *  Created on: 2015 márc. 10
 *      Author: Richárd
 */
#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#include "can_comm.h"
#include "light.h"

#define CAN_MIN_EID         0x10
#define CAN_MAX_EID         0x1FFFFFF

#define CAN_SM_MIN          0x10
#define CAN_SM_MAX          0x1F
#define CAN_SM_RIGHT        0x01
#define CAN_SM_LEFT         0x02
#define CAN_SM_W_LIGHTS     0x03

#define CAN_ML_MIN          0x20
#define CAN_ML_MAX          0x2F

#define CAN_RPY_MIN         0x30
#define CAN_RPY_MAX         0x3F

#define CAN_LC_MIN          0x40
#define CAN_LC_MAX          0x5F

enum canState
{
  CAN_SM,
  CAN_ML,
  CAN_RPY,
  CAN_LC,
  CAN_WAIT,
  CAN_NUM_CH
}canstate;

static CANTxFrame txmsg;
static CANRxFrame rxmsg;

static const CANConfig cancfg = {
  CAN_MCR_ABOM,
  CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
  CAN_BTR_TS1(8) | CAN_BTR_BRP(6)
};

static uint16_t id;
static uint16_t messages;

static WORKING_AREA(can_rx_wa, 256);
static msg_t can_rx(void *p) {
  EventListener el;

  (void)p;
  chRegSetThreadName("receiver");
  chEvtRegister(&CAND1.rxfull_event, &el, 0);
  while(!chThdShouldTerminate()) {
    if (chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(100)) == 0)
      continue;
    while (canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == RDY_OK) {


      id = rxmsg.EID >> 8;
      messages = (uint8_t)rxmsg.EID;

      if (id < CAN_MIN_EID && id > CAN_MAX_EID )
      {
        id = 0;
        messages = 0;
      }
      
      else if(id >= CAN_SM_MIN && id <= CAN_SM_MAX){
        canstate = CAN_SM;
      }
      else if(id >= CAN_ML_MIN && id <= CAN_ML_MAX){
        canstate = CAN_ML;
      }
      else if(id >= CAN_RPY_MIN && id <= CAN_RPY_MAX){
        canstate = CAN_RPY;
      }
      else if(id >= CAN_LC_MIN && id <= CAN_LC_MAX){
        canstate = CAN_LC;
      }
      else{
        
        canstate = CAN_WAIT;
      }

      switch(canstate){
        case CAN_SM:
          
          if(messages == CAN_SM_RIGHT){
            lightRight();
          }
          if(messages == CAN_SM_LEFT){
            lightLeft();
          }
          if(messages == CAN_SM_W_LIGHTS){
            lightWarning();
          }
          canstate = CAN_WAIT;
          break;

        case CAN_ML:
          canstate = CAN_WAIT;
          break;

        case CAN_RPY:
          canstate = CAN_WAIT;
          break;

        case CAN_LC:
          canstate = CAN_WAIT;
          break;
        case CAN_WAIT:
          break;

        default:
          break;
        }

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

void cmd_can_commvalues(BaseSequentialStream *chp, int argc, char *argv[]) {
  
  (void)argc;
  (void)argv;
  chprintf(chp, "\x1B\x63");
  chprintf(chp, "\x1B[2J");
  while (chnGetTimeout((BaseChannel *)chp, TIME_IMMEDIATE) == Q_TIMEOUT) {
    chprintf(chp, "\x1B[%d;%dH", 0, 0);
    chprintf(chp,"RX.EID : %x \r\n", rxmsg.EID);
    chprintf(chp,"id : %x \r\n", id);
    chprintf(chp,"messages : %x \r\n", messages);
    chprintf(chp,"\r\n");
    chprintf(chp,"RX.DATA1 : %d \r\n", rxmsg.data16[0]);
    chprintf(chp,"RX.DATA2 : %d \r\n", rxmsg.data16[1]);
    chprintf(chp,"RX.DATA3 : %d \r\n", rxmsg.data16[2]);
    chprintf(chp,"RX.DATA4 : %d \r\n", rxmsg.data16[3]);

    chThdSleepMilliseconds(1000);
  }
}