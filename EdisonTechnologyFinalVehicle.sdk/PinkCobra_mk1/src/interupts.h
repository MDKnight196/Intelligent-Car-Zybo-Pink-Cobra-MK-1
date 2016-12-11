/*
 * interupts.h
 *
 *  Created on: Dec 5, 2016
 *      Author: ulab
 */

#ifndef SRC_INTERUPTS_H_
#define SRC_INTERUPTS_H_
#include <stdio.h>
#include "platform.h"
#include "xil_types.h"
#include "xsysmon.h"
#include "xtmrctr.h"
#include "xparameters.h"
#include "Xil_exception.h"
#include "xttcps.h"
#include "xil_io.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "xgpio.h"
#include "PIDcontroller.h"
#include "operationalMode.h"
#include "pixy.h"
#include "bluetooth.h"
#include "hullEffect.h"
#include "utrasonic.h"

//#define XPAR_FABRIC_AXI_TIMER_0_INTERRUPT_INTR 61
#define SYSMON_DEVICE_ID XPAR_SYSMON_0_DEVICE_ID
//TTC
#define TTC_DEVICE_ID1		XPAR_XTTCPS_0_DEVICE_ID
#define TTC_DEVICE_ID2		XPAR_XTTCPS_1_DEVICE_ID
#define TTC_INTR_ID1		    XPAR_XTTCPS_0_INTR
#define TTC_INTR_ID2		    XPAR_XTTCPS_1_INTR
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
//GPIO
#define GPIO_GPIO_ID	XPAR_AXI_GPIO_0_DEVICE_ID
#define INT_GPIO		XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR
//tmr
#define INT_TIMER			XPAR_FABRIC_AXI_TIMER_2_INTERRUPT_INTR
#define TIMER_ID			XPAR_AXI_TIMER_2_DEVICE_ID

XGpio gpio;
XTmrCtr timer;


void SIHandler(void *data, u8 TmrCtrNumber);
void clockHandler(void *data, u8 TmrCtrNumber);
void TMR_Intr_Handler(void *data, u8 TmrCtrNumber);
void  GPIO_Intr_Handler(void *InstancePtr);
void intrSetup();
int timerSetup(u16 DeviceId);
void setupIntGPIO(u16 DeviceId, u16 intId);
int SetUpInterruptSystem(XScuGic *XScuGicInstancePtr);
int ScuGicInterrupt_Init(u16 DeviceId);
void setUPAxiTimers();
XTtcPs setUpTTC();
int getCenter();

#endif /* SRC_INTERUPTS_H_ */