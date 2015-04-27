/*
 * can_comm.h
 *
 *  Created on: 2015 márc. 10
 *      Author: Richárd
 */

#ifndef CAN_COMM_H_
#define CAN_COMM_H_

void can_commInit(void);

void cmd_can_commvalues(BaseSequentialStream *chp, int argc, char *argv[]);

#endif /* CAN_COMM_H_ */
