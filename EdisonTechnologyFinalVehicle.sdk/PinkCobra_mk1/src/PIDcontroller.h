/*
 * PIDcontroller.h
 *
 *  Created on: Dec 5, 2016
 *      Author: ulab
 */

#ifndef SRC_PIDCONTROLLER_H_
#define SRC_PIDCONTROLLER_H_

void init_PID(float KP, float KI, float KD, int Max, int Min, int Mid);
int cal_PID(int input);

#endif /* SRC_PIDCONTROLLER_H_ */
