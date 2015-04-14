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
#include "one_vire_comm.h"

#define CAN_EID                 0x20

#define CAN_ONE_VIRE_MESSAGE    0x01

#define CAN_MIN_EID             0x10
#define CAN_MAX_EID             0x1FFFFFF

#define CAN_SM_MIN              0x10
#define CAN_SM_MAX              0x1F
#define CAN_SM_RIGHT            0x01
#define CAN_SM_LEFT             0x02
#define CAN_SM_W_LIGHTS         0x03

#define CAN_ML_MIN              0x20
#define CAN_ML_MAX              0x2F


#define CAN_RPY_MIN             0x30
#define CAN_RPY_MAX             0x3F

#define CAN_LC_MIN              0x40
#define CAN_LC_MAX              0x5F

enum can_rxState
{
  CAN_RX_SM,
  CAN_RX_ML,
  CAN_RX_RPY,
  CAN_RX_LC,
  CAN_RX_WAIT,
  CAN_RX_NUM_CH
}can_rxstate;

enum can_txState
{
  CAN_TX_ONE_VIRE,
  CAN_TX_NUM_CH
}can_txstate;

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
        can_rxstate = CAN_RX_SM;
      }
      else if(id >= CAN_ML_MIN && id <= CAN_ML_MAX){
        can_rxstate = CAN_RX_ML;
      }
      else if(id >= CAN_RPY_MIN && id <= CAN_RPY_MAX){
        can_rxstate = CAN_RX_RPY;
      }
      else if(id >= CAN_LC_MIN && id <= CAN_LC_MAX){
        can_rxstate = CAN_RX_LC;
      }
      else{
        
        can_rxstate = CAN_RX_WAIT;
      }

      switch(can_rxstate){
        case CAN_RX_SM:
          
          if(messages == CAN_SM_RIGHT){
            lightRight();
          }
          if(messages == CAN_SM_LEFT){
            lightLeft();
          }
          if(messages == CAN_SM_W_LIGHTS){
            lightWarning();
          }
          can_rxstate = CAN_RX_WAIT;
          break;

        case CAN_RX_ML:
          can_rxstate = CAN_RX_WAIT;
          break;

        case CAN_RX_RPY:
          can_rxstate = CAN_RX_WAIT;
          break;

        case CAN_RX_LC:
          can_rxstate = CAN_RX_WAIT;
          break;
        case CAN_RX_WAIT:
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
  
  while (!chThdShouldTerminate()) {
    int tx_status;

    for(tx_status = 0; tx_status < CAN_TX_NUM_CH; tx_status ++)
      switch(tx_status){
        case CAN_TX_ONE_VIRE:      

          txmsg.EID = CAN_ONE_VIRE_MESSAGE;
          txmsg.EID += CAN_EID << 8;

          /* Only one sensor works  */
          txmsg.data16[0] = one_vireGetValue(0);
          txmsg.data16[1] = one_vireGetValue(1);
          txmsg.data16[2] = one_vireGetValue(2);
          txmsg.data16[3] = one_vireGetValue(3);
 
          canTransmit(&CAND1, CAN_ANY_MAILBOX ,&txmsg, MS2ST(100));
          break;

        default:
          break;
    }
    chThdSleepMilliseconds(1000);
  }
  return 0;
}

void can_commInit(void){
  txmsg.IDE = CAN_IDE_EXT;
  txmsg.RTR = CAN_RTR_DATA;
  txmsg.DLC = 8;

  canStart(&CAND1, &cancfg);

  chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 7, can_tx, NULL);
  chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO, can_rx, NULL);
}

void cmd_can_commvalues(BaseSequentialStream *chp, int argc, char *argv[]) {
  
  (void)argc;
  (void)argv;
  chprintf(chp, "\x1B\x63");
  chprintf(chp, "\x1B[2J");
  while (chnGetTimeout((BaseChannel *)chp, TIME_IMMEDIATE) == Q_TIMEOUT) {
    chprintf(chp, "\x1B[%d;%dH", 0, 0);
    chprintf(chp,"-------- CAN BUS Receive --------\r\n");
    chprintf(chp,"RX.EID : %x \r\n", rxmsg.EID);
    chprintf(chp,"id : %x \r\n", id);
    chprintf(chp,"messages : %x \r\n", messages);
    chprintf(chp,"\r\n");
    chprintf(chp,"RX.DATA1 : %d \r\n", rxmsg.data16[0]);
    chprintf(chp,"RX.DATA2 : %d \r\n", rxmsg.data16[1]);
    chprintf(chp,"RX.DATA3 : %d \r\n", rxmsg.data16[2]);
    chprintf(chp,"RX.DATA4 : %d \r\n", rxmsg.data16[3]);
    chprintf(chp,"\r\n");
    chprintf(chp,"-------- CAN BUS Transmit --------\r\n");
    chprintf(chp,"TX.EID : %x \r\n", txmsg.EID);
    chprintf(chp,"\r\n");
    chprintf(chp,"TX.DATA1 : %d \r\n", txmsg.data16[0]);
    chprintf(chp,"TX.DATA2 : %d \r\n", txmsg.data16[1]);
    chprintf(chp,"TX.DATA3 : %d \r\n", txmsg.data16[2]);
    chprintf(chp,"TX.DATA4 : %d \r\n", txmsg.data16[3]);

    chThdSleepMilliseconds(1000);
  }
}