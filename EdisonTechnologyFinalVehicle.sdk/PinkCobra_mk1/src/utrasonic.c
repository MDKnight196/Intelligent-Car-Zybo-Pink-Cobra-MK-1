/*
 * utrasonic.c
 *
 *  Created on: Dec 6, 2016
 *      Author: ulab
 */

#include "utrasonic.h"

int fitDIS(int readDIS){
	static int distance=0;

	if(readDIS>distance)
		distance++;
	else if(readDIS<distance)
		distance--;

	return distance;
}