/*
 * can_comm.h
 *
 *  Created on: 2015 márc. 10
 *      Author: Richárd
 */

#ifndef CAN_COMM_H_
#define CAN_COMM_H_

static msg_t can_tx(void * p);
static msg_t can_rx(void *p);
static msg_t Villog();
void can_commInit(void);



#endif /* CAN_COMM_H_ */
