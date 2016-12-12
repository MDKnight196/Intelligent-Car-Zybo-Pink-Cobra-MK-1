/*
 * operationalMode.c
 *
 *  Created on: Dec 7, 2016
 *      Author: ulab
 */

#include "operationalMode.h"
#include "xsysmon.h"
#include "xtmrctr.h"
#include "xparameters.h"
#include "Xil_exception.h"
#include "xttcps.h"
#include "xil_io.h"
#include "utrasonic.h"



int servoValue;
int forwardValue;
int backwardsValue;

int pixleCount; //passed in from interupts
int previousMode;
int startLineCount=0;
int delayValue=1;
int PixeyDelay=0;
int offset;
int cycleCount;
int mode=StandByMode;
int modeSel=StandByMode;
int debug=-1;
int MSpeed=0;
int MDirection=0;
int conn=-1;
int distance;
void setStandbyMode(){
	forwardValue=0;
	backwardsValue =0;
	servoValue= 2538;
	mode = StandByMode;
}
void setSpeedMode(){
	forwardValue=75;
	backwardsValue =0;
	mode = SpeedMode;
}
void setAcuracyMode(){
	forwardValue=50;
	backwardsValue =0;
	mode = AcuracyMode;
}
void setDiscoveryMode(){
	servoValue=2115;
	forwardValue=25;
	backwardsValue =0;
	mode = DiscoveryMode;
}
void setManualMode(){
	servoValue= 2538;
	forwardValue=0;
	backwardsValue =0;
	mode = ManualMode;
}
void setFollowMode(){
	forwardValue=70;
	backwardsValue =0;
	mode = AcuracyMode;
}
void updateMode(){
	if(mode == StandByMode){
		if(modeSel == DebugMode){
			debug=1;
			incModeSel();
		}
		else if(modeSel != StandByMode){
			mode=modeSel;
		}
	}else{
		display(StandByMode+1);
		mode=StandByMode;
		modeSel=StandByMode;
		debug=-1;
	}
}
void incModeSel(){
	int reset;
	if(mode == StandByMode){
		if (debug != 1){
			reset=DebugMode;
		}else{
			reset=DebugMode-1;
		}
		if(modeSel >= reset ){
			modeSel=StandByMode+1;
		}
		else{
			modeSel++;
		}
		display(modeSel+1);
	}
}


void ModeOperation(XTtcPs ForwardPWM, XTtcPs BackwardPWM, XTtcPs ServoTimer, int PWMSignal, int center, int blackPixelCount){
/*	if (delayValue < PixeyDelay ){

	}
	else{*/
		if( mode == StandByMode){
			XTtcPs_SetMatchValue(&ForwardPWM, 0, 21666);
			XTtcPs_SetMatchValue(&BackwardPWM, 0, 21666);
			XTtcPs_SetMatchValue(&ServoTimer, 0, 2538);
			previousMode=StandByMode;
			startLineCount=0;
			cycleCount=0;
		}
		else if (mode == SpeedMode){
			if (previousMode!=SpeedMode){
				XTtcPs_SetMatchValue(&ForwardPWM, 0, 13500);
				XTtcPs_SetMatchValue(&BackwardPWM, 0, 21666);
			}
			else if (blackPixelCount > 10 && blackPixelCount <30){
				offset=cal_PID(center);
				PWMSignal=2538 + offset;
				if(offset < 0 ){
					offset = -offset;
				}
				XTtcPs_SetMatchValue(&ForwardPWM, 0, 13500 + offset*2);
				XTtcPs_SetMatchValue(&ServoTimer, 0, PWMSignal); //update steering
				}
			else if (blackPixelCount> 35){
				if (startLineCount == 5){
					XTtcPs_SetMatchValue(&ForwardPWM, 0, 21666);
					XTtcPs_SetMatchValue(&BackwardPWM, 0, 21666);
					updateMode();
				}
				startLineCount++;
			}
/*			else if (blackPixelCount > 110){
			//	XTtcPs_SetMatchValue(&ForwardPWM, 0, 21666);
			//	XTtcPs_SetMatchValue(&BackwardPWM, 0, 21666);
			}*/
			previousMode=SpeedMode;
		}
		else if (mode == AcuracyMode){
				distance =getDIS();
			if (previousMode!=AcuracyMode){
				XTtcPs_SetMatchValue(&ForwardPWM, 0, 15166);
				XTtcPs_SetMatchValue(&BackwardPWM, 0, 21666);
			}
			if (blackPixelCount > 10 && blackPixelCount <30){
				offset=cal_PID(center);
				PWMSignal=2538+offset;
				if(offset > 0 ){
					offset = -offset;
				}
				if(distance < 24){
					XTtcPs_SetMatchValue(&ForwardPWM, 0, 21666);
					XTtcPs_SetMatchValue(&ServoTimer, 0, PWMSignal); //update steering
				}
				else{
				XTtcPs_SetMatchValue(&ForwardPWM, 0, 15166);
				XTtcPs_SetMatchValue(&ServoTimer, 0, PWMSignal); //update steering
				}
				}
			else if (blackPixelCount> 30){
				if (startLineCount == 5){
					XTtcPs_SetMatchValue(&ForwardPWM, 0, 21666);
					XTtcPs_SetMatchValue(&BackwardPWM, 0, 21666);
					updateMode();
				}
				startLineCount++;
			}
//			else if (blackPixelCount > 110){
/*				XTtcPs_SetMatchValue(&ForwardPWM, 0, 21666);
				XTtcPs_SetMatchValue(&BackwardPWM, 0, 21666);*/
//			}
			previousMode=AcuracyMode;
		}
		else if  (mode == DiscoveryMode){
			if (previousMode!=DiscoveryMode){
				XTtcPs_SetMatchValue(&ForwardPWM, 0, 13050);
				XTtcPs_SetMatchValue(&BackwardPWM, 0, 21666);
				XTtcPs_SetMatchValue(&ServoTimer, 0, 2115);
			}
			if (blackPixelCount > 5 && blackPixelCount < 8){
				if (cycleCount < 10){
					mode =AcuracyMode;
					startLineCount=1;
				}
				else
					cycleCount++;
			}
		}
		else if (mode ==  ManualMode){
			if(conn==1){
				if(MSpeed > 0){
				XTtcPs_SetMatchValue(&ForwardPWM, 0, 21666-MSpeed*400);
				XTtcPs_SetMatchValue(&BackwardPWM, 0, 21666);
				}
				else
				{
					XTtcPs_SetMatchValue(&ForwardPWM, 0, 21666);
					XTtcPs_SetMatchValue(&BackwardPWM, 0, 21666+MSpeed*400);
				}
				XTtcPs_SetMatchValue(&ServoTimer, 0, 2538 + MDirection *8);
			}
			else{
				XTtcPs_SetMatchValue(&ForwardPWM, 0, 21666);
				XTtcPs_SetMatchValue(&BackwardPWM, 0, 21666);
				XTtcPs_SetMatchValue(&ServoTimer, 0, 2538);
			}
		}
		else if (mode == DebugMode){
			if (previousMode!=AcuracyMode){
							XTtcPs_SetMatchValue(&ForwardPWM, 0, 15166);
							XTtcPs_SetMatchValue(&BackwardPWM, 0, 21666);
				}
			if (blackPixelCount > 24 && blackPixelCount <30){

					PWMSignal=2538+ cal_PID(center);;

							XTtcPs_SetMatchValue(&ForwardPWM, 0, 15166 + offset);
							XTtcPs_SetMatchValue(&ServoTimer, 0, PWMSignal); //update steering
			}
		}
		else if (mode == CollisionAvoidanceLeader	){
			//read in ultraSonic and slow car down based on distance
		}
		else{

		}
//}

}

int isDebug(){
	return debug;
}

int getMode(){
	return mode;
}

void setManualControl(int speed, int direction){
	MSpeed=speed;
	MDirection=direction;

}

void setConnect(int Connect){
	conn=Connect;
}
