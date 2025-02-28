/*
 * BLDCMotor.c
 *
 *  Created on: May 29, 2024
 *      Author: 45951
 */
#include "BLDCMotor.h"
struct Motor
{
	uint8_t IsPowerOn;//电机电源
	uint8_t IsReset;//电机复位
	double EndPosition;//运动末尾
	double ChargingPosition;//充电位置
	double distance;////电机移动距离
	int32_t pulses;//电机脉冲
	int8_t	direction;//电机运动方向
	uint16_t DutyRatio;//PWM占空比
	int16_t	speed;//电机速度
};
struct Motor BLDCMotor;
uint8_t Flag_Events_BLDCMotor = 0;
uint8_t Flag_Event_Init_BLDCMotor = 0;
uint8_t Flag_Event_Action_BLDCMotor = 0;
void Events_BLDCMotor(void)
{
	switch(Flag_Events_BLDCMotor)
	{
		case 0:
		{//如果第一次给电机上电，那就是这个状态
			Event_Init_BLDCMotor();
		}break;
		case 1:
		{
			Event_Action_BLDCMotor();
		}break;
	}
}

void Event_Init_BLDCMotor(void)
{
	switch(Flag_Event_Init_BLDCMotor)
	{
		case 0:
		{
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
			Flag_Event_Init_BLDCMotor = 1;
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 50);
		}break;
		case 1:
		{
			Flag_Event_Init_BLDCMotor = 2;
			Flag_Events_BLDCMotor = 1;
		}break;
	}
}

void Event_Action_BLDCMotor(void)
{
	switch(Flag_Event_Action_BLDCMotor)
	{
		case 0:
		{
			uint8_t test = HAL_GPIO_ReadPin(Input_EmergencyStopButton_GPIO_Port, Input_EmergencyStopButton_Pin);
			uint8_t test1 = HAL_GPIO_ReadPin(Input_Sensor1_GPIO_Port, Input_Sensor1_Pin);
			uint8_t test2 = HAL_GPIO_ReadPin(Input_Sensor2_GPIO_Port, Input_Sensor2_Pin);
			if(test == 1)
			{
				HAL_GPIO_WritePin(Power_EStopLED_GPIO_Port, Power_EStopLED_Pin, 0);
				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
			}
			else
			{
				HAL_GPIO_WritePin(Power_EStopLED_GPIO_Port, Power_EStopLED_Pin, 1);
				__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 100);
			}

			if(test1 == 1)
				HAL_GPIO_WritePin(Direction_Ctl_BLDCMotor_GPIO_Port, Direction_Ctl_BLDCMotor_Pin, 0);
			else if(test2 == 1)
				HAL_GPIO_WritePin(Direction_Ctl_BLDCMotor_GPIO_Port, Direction_Ctl_BLDCMotor_Pin, 1);

		}break;
		case 1:
		{

		}break;
	}
}

double GetStatus_CurrentPosition_BLDCMotor(void)
{
	//BLDCMotor.distance = BLDCMotor.pulses / Pulses_Per_Revolution;
	return BLDCMotor.distance;
}

void SetStatus_Reset_BLDCMotor(uint8_t status)
{
	BLDCMotor.IsReset = status;
}
uint8_t GetStatus_Reset_BLDCMotor(void)
{
	return BLDCMotor.IsReset;
}
void SetStatus_Power_BLDCMotor(uint8_t status)
{
	HAL_GPIO_WritePin(Power_BLDCMotor_GPIO_Port, Power_BLDCMotor_Pin, status);
	BLDCMotor.IsPowerOn = Power_BLDCMotor_Pin == PowerON_BLDCMotor ? 1 : 0;
}
uint8_t GetStatus_Power_BLDCMotor(void)
{
	return BLDCMotor.IsPowerOn;
}
