/*
	Modulux - Copyright (C) 2015
    GAMF MegaLux Team
 */

#ifndef CAN_COMM_H_
#define CAN_COMM_H_

void can_commInit(void);

void cmd_can_commvalues(BaseSequentialStream *chp, int argc, char *argv[]);

#endif /* CAN_COMM_H_ */
