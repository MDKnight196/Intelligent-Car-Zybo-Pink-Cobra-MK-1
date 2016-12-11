/*
 * pixy.h
 *
 *  Created on: Dec 6, 2016
 *      Author: ulab
 */

#ifndef SRC_PIXY_H_
#define SRC_PIXY_H_

#include "xuartps.h"

#define YELLOW				0x000a
#define GREEN				0x000b
#define BUFFER_SIZE 		100
#define PIXY_ID				XPAR_PS7_UART_1_DEVICE_ID

struct Blocks{
	int cc1_set;
	u16 cc1_sig;
	u16	cc1_loc;
	int cc2_set;
	u16 cc2_sig;
	u16 cc2_loc;
	int read;
};

XUartPs pixy;

int pixySetup(u16 DeviceId);
struct Blocks getBlocks();
int getDirection(struct Blocks check, struct Blocks *read, int *cnt);


#endif /* SRC_PIXY_H_ */
