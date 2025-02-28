/*
 * BLDCMotor.h
 *
 *  Created on: May 29, 2024
 *      Author: 45951
 */

#ifndef INC_BLDCMOTOR_H_
#define INC_BLDCMOTOR_H_

#include "main.h"

#define Pulses_Per_Revolution 9	//每转脉冲数值
#define Reduction_Ratio 125		//减速比
#define Belt_Pitch_mm 2			//同步带节距2mm

#define PowerON_BLDCMotor 0
#define PowerOFF_BLDCMotor 1

void Events_BLDCMotor(void);
void Event_Init_BLDCMotor(void);
void Event_Action_BLDCMotor(void);

void SetStatus_Reset_BLDCMotor(uint8_t status);
uint8_t GetStatus_Reset_BLDCMotor(void);
void SetStatus_Power_BLDCMotor(uint8_t status);
uint8_t GetStatus_Power_BLDCMotor(void);

#endif /* INC_BLDCMOTOR_H_ */
