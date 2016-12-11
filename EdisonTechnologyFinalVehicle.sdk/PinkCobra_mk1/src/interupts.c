/*
 * interupts.c
 *
 *  Created on: Dec 5, 2016
 *      Author: ulab
 */
#include "interupts.h"


int pixelCount=0;
static XSysMon SysMonInst;
XSysMon_Config *SysMonConfigPtr;
XSysMon *SysMonInstPtr = &SysMonInst;
int turnRight;
int turnLeft;
int direction;
XScuGic InterruptController; /* Instance of the Interrupt Controller */
static XScuGic_Config *GicConfig;/* The configuration parameters of the
controller */
int index1 = 0;
u16 PWMinterval;
u16 PWMintervalMin;
u16 PWMintervalMax;
extern int center;
int tempCenter;
int turnValue=0;
int previousTurnValue=-1;
int blackPixelCount=0;
int PWMSignal;
XTtcPs_Config *Config;
XTtcPs Timer;
XTtcPs ForwardPWM;
XTtcPs BackwardPWM;
XTtcPs ServoTimer;
int opMode = 0;
int opMode_select = STANDBY_IMAGE_NUM;
int manSpeed=0;
int manTurn=0;
float standardDivation=0;

typedef struct {
	u32 OutputHz;	/* Output frequency */
	u16 Interval;	/* Interval value */
	u8 Prescaler;	/* Prescaler value */
	u16 Options;	/* Option settings */
	u16 Width;
} TmrCntrSetup;

static TmrCntrSetup SettingsTable[3] = {
	{5000, 0, 16, 84, 230},	/* Ticker timer counter initial setup, only output freq */
	{5000, 0, 16, 84, 230},
	{50, 0, 16, 84, 230}
};




void SIHandler(void *data, u8 TmrCtrNumber)
{
	XTmrCtr_Stop(data,TmrCtrNumber);
	XTmrCtr_Reset(data,TmrCtrNumber);
	if (isDebug() == 1){
		printf("\n\r");
	}
	pixelCount=0;
	XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, 0, 0x000006D4);
	XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, 1, 0x00000694);
XTmrCtr_Start(data,TmrCtrNumber);

}
void clockHandler(void *data, u8 TmrCtrNumber)
{

	int tempADC;
//	u32 StatusEvent;
	if (isDebug() == 1){
		printf("\n\r back here!");
	}
	XTmrCtr_Stop(data,TmrCtrNumber);
	XTmrCtr_Reset(data,TmrCtrNumber);
if(pixelCount < 1){
	pixelCount++;
	XSysMon_StartAdcConversion(SysMonInstPtr);
	XTmrCtr_Start(data,TmrCtrNumber);
}
else if (pixelCount < 129){
	tempADC =XSysMon_GetAdcData(SysMonInstPtr,30);
	XSysMon_StartAdcConversion(SysMonInstPtr);
	//while ((XSysMon_GetStatus(SysMonInstPtr) & XSM_SR_EOS_MASK) != XSM_SR_EOS_MASK);
			if (isDebug() == 1){
				printf("\n\r %d \n\r", tempADC);
			}
			if( tempADC<7500){
				blackPixelCount++;
				tempCenter = tempCenter + pixelCount -1;
				if (isDebug() == 1){
					printf("#");
				}
			}
			else{
				if (isDebug() == 1){
					printf(" ");
				}
			}
			pixelCount++;
			XTmrCtr_Start(data,TmrCtrNumber);
		}
	else if (pixelCount == 129){
		center= tempCenter/blackPixelCount;
		tempCenter = 0;
		if (isDebug() == 1){
			printf("%d\n\r", center);
		}
		pixelCount++;
		ModeOperation(ForwardPWM, BackwardPWM, ServoTimer, PWMSignal, center, blackPixelCount);
/*		PWMSignal=2538 + cal_PID(center);
		XTtcPs_SetMatchValue(&ServoTimer, 0, PWMSignal);*/

		blackPixelCount=0;
		XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, 0, 0x00000000);
		XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, 1, 0x00000000);
	}
}

void TMR_Intr_Handler(void *data, u8 TmrCtrNumber)
{
	static struct Blocks blocks, lastBlocks;
	static struct Blocks rBlocks[5];
	static int ms=0;
	int k = 0;
	static int  cnt;



		blocks = getBlocks();
		if(cnt==0){
			if(blocks.read == 1 ){
				lastBlocks=blocks;
			}
		}
		rBlocks[cnt]=blocks;
		cnt++;

		if(cnt==5){
			direction=getDirection(lastBlocks, &rBlocks[0], &cnt);
		}

		if(ms==50){
			k=0;
			k=addToString(opMode, k);
			k=addToString(direction, k);
			k=addToString(getDIS(), k);

			k=addToString(getDISTraveled(intCnt), k);
			k=addToString(calSpeed(intCnt), k);
			k=newline( k);
			//writeData(&bluetooth, &outputBuffer[0], k);
			XUartPs_Send(&bluetooth, &outputBuffer[0], k);
			ms=0;
		}
		else{
			ms++;
		}

	if(getMode() == ManualMode){
		XUartPs_Recv(&bluetooth, &inputBuffer[0], 7);
		if(inputBuffer[0]=='^' && inputBuffer[1] == 'C' && inputBuffer[2]=='O' && inputBuffer[3] == 'N' && inputBuffer[4] =='N'){
			setConnect(1);
			}
		else if (inputBuffer[6] == '#'){
			manSpeed=(inputBuffer[0] == '+' ? ((inputBuffer[1]-'0')*10)+(inputBuffer[2]-'0'):(-1*((inputBuffer[1]-'0')*10)+(inputBuffer[2]-'0')));
			if(manSpeed>50){
				manSpeed=50;
			}else if(manSpeed <-50){
				manSpeed=-50;
			}
			manTurn=inputBuffer[3] == '+' ? ((inputBuffer[4]-'0')*10)+(inputBuffer[5]-'0'):(-1*((inputBuffer[4]-'0')*10)+(inputBuffer[5]-'0'));
			if(manTurn>50){
				manTurn=50;
			}else if(manTurn <-50){
				manTurn=-50;
			}
			setManualControl(manSpeed, manTurn);
		}
		else if(inputBuffer[0]=='^' && inputBuffer[1] =='D' && inputBuffer[2] == 'I' && inputBuffer[3] == 'S' && inputBuffer[4] == 'C' &&
				inputBuffer[5]=='O' && inputBuffer[6] == 'N' && inputBuffer[7] =='N'){
			setConnect(-1);
		}
	}
	XTmrCtr_Stop(data,TmrCtrNumber);
	XTmrCtr_Reset(data,TmrCtrNumber);
	XTmrCtr_Start(data,TmrCtrNumber);

}

void GPIO_Intr_Handler(void *InstancePtr)
{
	int GPIO_value=0;
	static int GPIOLast=0;
	static int encoderLast=0b000;

	//disable GPIO interrupts
	XGpio_InterruptDisable(&gpio, INT_GPIO);
	//Ignore additional button process
	if ((XGpio_InterruptGetStatus(&gpio) & 0x1) != 0x1)
	{
		return;
	}

	GPIO_value = XGpio_DiscreteRead(&gpio, 1);
	int test1 = GPIO_value & 0b00000000010;
	int test2 = GPIOLast & 0b00000000010;
	if ((GPIO_value & 0b00000000001) == 0 && (GPIOLast & 0b00000000001) == 1){
		incModeSel();
	}
	else if((GPIO_value & 0b00000000010) == 0 && (GPIOLast & 0b00000000010) == 2){
		updateMode();
	}
	else if((GPIO_value & 0b00000000100) != encoderLast && opMode != 0){
		intCnt++;
	}
	else{
		fitDIS(GPIO_value>>3);

	}


	GPIOLast=GPIO_value;
	(void)XGpio_InterruptClear(&gpio, INT_GPIO);
	//enable GPIO interrupts
	XGpio_InterruptEnable(&gpio, INT_GPIO);
}


int SetUpInterruptSystem(XScuGic *XScuGicInstancePtr)
{
/*
* Connect the interrupt controller interrupt handler to the hardware
* interrupt handling logic in the ARM processor.
*/
Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
(Xil_ExceptionHandler) XScuGic_InterruptHandler,
XScuGicInstancePtr);
/*
* Enable interrupts in the ARM
*/
Xil_ExceptionEnable();

return XST_SUCCESS;
}
int ScuGicInterrupt_Init(u16 DeviceId)
{
int Status;
/*
* Initialize the interrupt controller driver so that it is ready to
* use.
* */


GicConfig = XScuGic_LookupConfig(DeviceId);
if (NULL == GicConfig) {
return XST_FAILURE;
}
Status = XScuGic_CfgInitialize(&InterruptController, GicConfig,
GicConfig->CpuBaseAddress);
if (Status != XST_SUCCESS) {
return XST_FAILURE;
}
/*
* Setup the Interrupt System
* */
Status = SetUpInterruptSystem(&InterruptController);
if (Status != XST_SUCCESS) {
return XST_FAILURE;
}



return XST_SUCCESS;
}

void intrSetup(){
	ScuGicInterrupt_Init(XPAR_PS7_SCUGIC_0_DEVICE_ID);
	setupIntGPIO(GPIO_GPIO_ID, INT_GPIO);
//	timerSetup(PIXY_ID);


}


void setupIntGPIO(u16 DeviceId, u16 intId){
	XGpio_Initialize(&gpio, DeviceId);
	XGpio_SetDataDirection(&gpio, 1, 0x3ff); //10 bits in


	XGpio_InterruptEnable(&gpio, 1);
	XGpio_InterruptGlobalEnable(&gpio);

	XScuGic_Connect(&InterruptController, INT_GPIO, (Xil_ExceptionHandler) GPIO_Intr_Handler, &gpio);
	XScuGic_Enable(&InterruptController, INT_GPIO);

}

/*int timerSetup(u16 DeviceId)
{
	XTmrCtr SystemTimer;

	int Status;
Status = XTmrCtr_Initialize(&SystemTimer , XPAR_AXI_TIMER_2_DEVICE_ID);
	if(XST_SUCCESS != Status)
	   	return XST_FAILURE;

	XTmrCtr_SetHandler(&SystemTimer, TMR_Intr_Handler, &SystemTimer);

	XTmrCtr_SetResetValue(&SystemTimer, 0, 0xFFE17B7F);//Change with generic value
	// 0xFFFE7961 0xFFF0BDBF 0xFA0A1EFF
	//1ms			10ms		1s
	XTmrCtr_SetOptions(&SystemTimer, DeviceId, (XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION ));

	XScuGic_Connect(&InterruptController, INT_TIMER, (Xil_ExceptionHandler) XTmrCtr_InterruptHandler, &SystemTimer);
	XScuGic_Enable(&InterruptController, INT_TIMER);

	XTmrCtr_Start(&SystemTimer, 0);
	return XST_SUCCESS;
}*/


void setUPAxiTimers(){
	XTmrCtr TimerClock;
	XTmrCtr TimerSI;
	XTmrCtr SystemTimer;
	int xStatus;

	init_platform();

	//**********************XADC
	SysMonConfigPtr = XSysMon_LookupConfig(SYSMON_DEVICE_ID);
	if(SysMonConfigPtr == NULL && isDebug()==1) printf("LookupConfig FAILURE\n\r");
	xStatus = XSysMon_CfgInitialize(SysMonInstPtr, SysMonConfigPtr,SysMonConfigPtr->BaseAddress);
	if(XST_SUCCESS != xStatus && isDebug()==1) printf("CfgInitialize FAILED\r\n");
	XSysMon_GetStatus(SysMonInstPtr);

	//*************************
	if (isDebug() == 1){
		printf("##### Application Starts #####\n\r");
		printf("\r\n");
	}
	xStatus = XTmrCtr_Initialize(&TimerClock,XPAR_AXI_TIMER_0_DEVICE_ID);
	if(XST_SUCCESS != xStatus && isDebug()==1)
	print("TIMER CLOCK INIT FAILED \n\r");
	xStatus = XTmrCtr_Initialize(&TimerSI,XPAR_AXI_TIMER_1_DEVICE_ID);
	if(XST_SUCCESS != xStatus && isDebug()==1)
	print("TIMER CLOCK INIT FAILED \n\r");
	xStatus = XTmrCtr_Initialize(&SystemTimer , XPAR_AXI_TIMER_2_DEVICE_ID);
	if(XST_SUCCESS != xStatus && isDebug()==1)
	print("SYSTEM TIMER INIT FAILED \n\r");


	//xStatus = XTmrCtr_Initialize(&ServoPWM,XPAR_AXI_TIMER_2_DEVICE_ID);
	//if(XST_SUCCESS != xStatus)
	//print("ServoPWM INIT FAILED \n\r");

	XTmrCtr_SetHandler(&TimerClock,	clockHandler,	&TimerClock);
	XTmrCtr_SetHandler(&TimerSI,	SIHandler,	&TimerSI);
	XTmrCtr_SetHandler(&SystemTimer, TMR_Intr_Handler, &SystemTimer);



	XTmrCtr_WriteReg(XPAR_AXI_TIMER_0_BASEADDR, 0,  XTC_TLR_OFFSET, 0xFFFFE5F4);
	XTmrCtr_WriteReg(XPAR_AXI_TIMER_0_BASEADDR, 1,  XTC_TLR_OFFSET, 0xFFFFF2F9 );
	XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, 0, 0x000002D4);
	XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_0_BASEADDR, 1, 0x00000694);

	XTmrCtr_WriteReg(XPAR_AXI_TIMER_1_BASEADDR, 0,  XTC_TLR_OFFSET, 0xFFE17B7F );
	XTmrCtr_WriteReg(XPAR_AXI_TIMER_1_BASEADDR, 1,  XTC_TLR_OFFSET, 0xFFFFBEE4  );
	XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR, 0, 0x000002D4);
	XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_1_BASEADDR, 1, 0x00000694);

	XTmrCtr_WriteReg(XPAR_AXI_TIMER_2_BASEADDR, 0,  XTC_TLR_OFFSET, 0xFFE17B7F );
	XTmrCtr_WriteReg(XPAR_AXI_TIMER_2_BASEADDR, 1,  XTC_TLR_OFFSET, 0xFFFFBEE4 );
	XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_2_BASEADDR, 0, 0x000002D4);
	XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_2_BASEADDR, 1, 0x00000694);
	//XTmrCtr_WriteReg(XPAR_AXI_TIMER_2_BASEADDR, 1,  XTC_TLR_OFFSET, 0xFFFDB60F );
	//XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_2_BASEADDR, 0, 0x00000294);
	//XTmrCtr_SetControlStatusReg(XPAR_AXI_TIMER_2_BASEADDR, 1, 0x00000694);

	//xStatus=ScuGicInterrupt_Init(XPAR_PS7_SCUGIC_0_DEVICE_ID);
	//if(XST_SUCCESS != xStatus)
	//print(" :( SCUGIC INIT FAILED \n\r");

/*	XTmrCtr_SetResetValue(&SystemTimer, 0, 0xFFE17B7F);
	XTmrCtr_SetOptions(&SystemTimer, XPAR_AXI_TIMER_2_BASEADDR, (XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION ));*/

	xStatus = XScuGic_Connect(&InterruptController, XPAR_FABRIC_AXI_TIMER_0_INTERRUPT_INTR, (Xil_ExceptionHandler)XTmrCtr_InterruptHandler, (void *)&TimerClock);
	xStatus = XScuGic_Connect(&InterruptController, XPAR_FABRIC_AXI_TIMER_1_INTERRUPT_INTR, (Xil_ExceptionHandler)XTmrCtr_InterruptHandler,(void *)&TimerSI);
	xStatus = XScuGic_Connect(&InterruptController, XPAR_FABRIC_AXI_TIMER_2_INTERRUPT_INTR, (Xil_ExceptionHandler)XTmrCtr_InterruptHandler,(void *)&SystemTimer);

	XScuGic_Enable(&InterruptController, XPAR_FABRIC_AXI_TIMER_0_INTERRUPT_INTR);
	XScuGic_Enable(&InterruptController, XPAR_FABRIC_AXI_TIMER_1_INTERRUPT_INTR);
	XScuGic_Enable(&InterruptController, XPAR_FABRIC_AXI_TIMER_2_INTERRUPT_INTR);
	//Start Timers
	XTmrCtr_Start(&TimerClock,0);
	XTmrCtr_Start(&TimerClock,1);
	XTmrCtr_Start(&TimerSI,0);
	XTmrCtr_Start(&TimerSI,1);
	XTmrCtr_Start(&SystemTimer,0);
	XTmrCtr_Start(&SystemTimer,1);
	if (isDebug() == 1){
		printf("timer start \n\r");
	}
}


XTtcPs setUpTTC(){
	int count;
	TmrCntrSetup *TimerSetup;
	u16 interval;

	for (count =0; count < 3; count++){

			 	TimerSetup = &SettingsTable[count];
			 	Config = XTtcPs_LookupConfig(count);
		 		XTtcPs_CfgInitialize(&Timer, Config, Config->BaseAddress);
			 	XTtcPs_Stop(&Timer);


		 		TimerSetup->Options |= (XTTCPS_OPTION_INTERVAL_MODE |XTTCPS_OPTION_MATCH_MODE);
			 	XTtcPs_SetOptions(&Timer, TimerSetup->Options);
			 	XTtcPs_CalcIntervalFromFreq(&Timer, TimerSetup->OutputHz,&(TimerSetup->Interval), &(TimerSetup->Prescaler));
			    XTtcPs_SetInterval(&Timer, TimerSetup->Interval);
			    XTtcPs_SetPrescaler(&Timer, TimerSetup->Prescaler);
			    if( count == 0)
			    	{
			    	interval=TimerSetup->Interval;
			    	ForwardPWM = Timer;
			    	}
			    else if( count == 1)
			    	{
			    	interval=TimerSetup->Interval;
			    	BackwardPWM = Timer;
			    	}
			    else if (count == 2 ){
			    	ServoTimer=Timer;
			    	PWMintervalMin=TimerSetup->Interval/20;
			    	PWMintervalMax=TimerSetup->Interval/10;
			    	PWMinterval=(PWMintervalMin + PWMintervalMax)/2;
			    	interval =PWMinterval;
			    }
			    if (isDebug() == 1){
			    	printf("%d\r\n", interval);
			    }
			    XTtcPs_SetMatchValue(&Timer, 0, interval);

			    XTtcPs_Start(&Timer);
			 	}
	return ServoTimer;
}
