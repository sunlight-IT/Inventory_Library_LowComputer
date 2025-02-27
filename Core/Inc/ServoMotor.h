/*
 * ServoMotor.h
 *
 *  Created on: Nov 1, 2024
 *      Author: 45951
 */

#ifndef INC_SERVOMOTOR_H_
#define INC_SERVOMOTOR_H_
#include "main.h"
#define huart_ServoMotor huart2
#define UartX_ServoMotor USART2

#define Power_ServoMotor_GPIO_Port GPIOA
#define Power_ServoMotor_Pin GPIO_PIN_8
#define PowerON_ServoMotor 0
#define PowerOFF_ServoMotor 1

#define LimitSensor_ServoMotor_GPIO_Port GPIOC
#define LimitSensor_ServoMotor_Pin GPIO_PIN_1
#define LimitSensorON_ServoMotor 0
#define LimitSensorOFF_ServoMotor 1

#define Left_Position_ServoMotor -1
#define Right_Position_ServoMotor 1

#define Cmd_Header_ServoMotor ':' // 0x3A

#define Cmd_Read_Func_ServoMotor 0x03
#define Cmd_Write_Once_Func_ServoMotor 0x06
#define Cmd_Write_Multi_Func_ServoMotor 0x10

void Events_ServoMotor(void);	  // 读写器事件
void Event_Init_ServoMotor(void); // 初始化事件
void Event_Movement_ServoMotor(void);
void TimerTasks_ServoMotor(void);
void ResetTimerFlag_ServoMotor(uint16_t *TimerFlag);
void SetStatus_Power_ServoMotor(uint8_t status);
uint8_t GetStatus_Power_ServoMotor(void);
void UartSend_MonitorRegister_ServoMotor(uint8_t mode);

// 串口
void UartWrite_ServoMotor(uint8_t *pData, uint16_t Size); // 串口发送函数
void UartReceiveIT_ServoMotor(void);					  // 读写器串口中断接收
void UartGetData_ServoMotor(void);						  // 处理接收到的数据到新的数组，并重新接收
void UartAction_ServoMotor(uint8_t *buf, uint8_t len);	  // 串口接收函数
uint8_t UartRead_ServoMotor(uint8_t *buf, uint8_t len);	  // 串口数据读取处理
void UartRxdMonitor_ServoMotor(uint8_t time_ms);		  // 串口超时接收监控函数
void UartDriver_ServoMotor(void);						  // 串口主循环驱动函数
void UartWrite_Add_MODBUS_CRC16_ServoMotor(uint8_t *buf, uint16_t len);

void Get_MODBUSCRC16_ServoMotor(uint8_t *buf, uint16_t len, uint8_t *crcl, uint8_t *crch);

void UartWrite_HexToAscii_AddLRC_ServoMotor(uint8_t *buf, uint8_t len);
void Uart_ReadWriteRegister_ServoMotor(uint8_t Read_WriteFunc, uint8_t WordsNum, uint16_t Register,
									   uint16_t Write_Once_ByteData, uint8_t *Write_Multi_Bytes);
void UartRegroup_ASCII_ServoMotor(void);
uint8_t Get_Regroup_ASCII(uint8_t data, uint8_t H_L);
#define MonitorRegister_1_Data 0x0012 // 状态监控缓存器1的读取起始位
#define MonitorRegister_1_Set 0x0022  // 状态监控缓存器1设置位
#define MonitorRegister_2_Set 0x0024  // 状态监控缓存器2设置位
#define MonitorRegister_3_Set 0x0026  // 状态监控缓存器3设置位
#define MonitorRegister_4_Set 0x0028  // 状态监控缓存器4设置位
#define MonitorRegister_5_Set 0x002A  // 状态监控缓存器5设置位
#define PulseMonitor_0022H 0x0000	  // 脉冲回授（状态监控设置）
#define TorqueMonitor_0024H 0x000B	  // 扭矩回授（状态监控设置）
#define SpeedMonitor_0026H 0x0007	  // 速度回授（状态监控设置）
#define Monitor_0028H 0x0003		  // 保留设置（状态监控设置）
#define Monitor_002AH 0x0000		  // 保留设置（状态监控设置）

#define GearRatioRegister_Numerator 0x0158	 // 电子齿轮比分子（N1）
#define GearRatioRegister_Denominator 0x015A // 电子齿轮比分母（M）
// 写入正确的电子齿轮比分子分母的设置，分子160，分母94，对应的皮带94mm/圈，计算得出94000脉冲/圈，1000脉冲/mm,原160000脉冲/圈
#define GearRatio_Numerator_0158H 0x000000A0   // 电子齿轮比分子（N1）160
#define GearRatio_Denominator_015AH 0x0000005E // 电子齿轮比分母（M）94

#define AuxrFuncRegister_P2_30 0x023C // 0:关闭所有下述功能;1：强制软件 Servo On。5：设定可防止连续写入EEPROM,若使用通讯控制时必需将此参数设定。
#define EEPROM_Disable_P2_30 0x0005

#define PulseReset_Register_P2_50 0x0264 // 脉冲清除模式，需要设置为0x11;
#define PulseReset_Data_P2_50 0x0011	 // 脉冲清除模式，需要设置为0x11;

#define DI1_Register_P2_10 0x0214 // DI1设置
#define DI1_Data_ServoON 0x0001	  // 常闭servo on
#define DI1_Data_ServoOFF 0x0101  // 常开servo off

#define JOGmode_Register_P4_05 0x040A // 寸动模式
#define JOGmode_Data_CW 0x1387		  // 顺时针转
#define JOGmode_Data_CCW 0x1386		  // 逆时针转

#define PT_Szmode_Register_P1_01 0x0102 // 控制模式及控制命令输入源设定,写入4，设置速度模式，写入0，设置位置模式
#define PTmode_Data_P1_01 0x0000		// 写入0，设置位置模式
#define Szmode_Data_P1_01 0x0004		// 写入4，设置速度模式

#define DI2_PulseClear_Register_P2_11 0x0216   // DI2设置0x0004-脉冲清除,0x0104	//脉冲恢复计数
#define DI2_PulseClear_Start_Data_P2_11 0x0004 // 0x0004-脉冲清除
#define DI2_PulseClear_End_Data_P2_11 0x0104   // 0x0104-脉冲恢复计数
void Uart_Send_MovementRegister_ServoMotor(uint8_t mode, uint16_t Data);

void test(uint8_t *buf);
#endif /* INC_SERVOMOTOR_H_ */
