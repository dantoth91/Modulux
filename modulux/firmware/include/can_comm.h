/*
 * can_comm.h
 *
 *  Created on: 2015 m�rc. 10
 *      Author: Rich�rd
 */

#ifndef CAN_COMM_H_
#define CAN_COMM_H_

void can_commInit(void);

void cmd_can_commvalues(BaseSequentialStream *chp, int argc, char *argv[]);

#endif /* CAN_COMM_H_ */
