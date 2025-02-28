/*
 * Reader15693.h
 *
 *  Created on: Oct 16, 2024
 *      Author: 45951
 */
//读写器一共有31条自定义命令，方便用户对读写器的操作。上位机在发送这些命令时，操作控制符（State）的高4位必须是“F”
#ifndef INC_READER15693_H_
#define INC_READER15693_H_
#include "main.h"
#define huart_Reader15693 	huart1
#define UartX_Reader15693	USART1

#define CleanBookLabels 1
#define KeepBookLabels 0


uint8_t GetStatus_Number_SavedBookLabels_Reader15693(void);
void SetStatus_Number_SavedBookLabels_Reader15693(int8_t ChangeNum,uint8_t CleanFlag);

uint8_t GetStatus_Is_x_PositionCorrected_Reader15693(void);
void SetStatus_Is_x_PositionCorrected_Reader15693(uint8_t status);
uint8_t GetStatus_Is_y_PositionCorrected_Reader15693(void);
void SetStatus_Is_y_PositionCorrected_Reader15693(uint8_t status);


void Events_Reader15693(void);//读写器事件
void Event_Init_Reader15693(void);//初始化事件
void TimerTasks_Reader15693(void);
//串口
void UartWrite_Reader15693(uint8_t *pData, uint16_t Size);//串口发送函数
void UartReceiveIT_Reader15693(void);//读写器串口中断接收
void UartGetData_Reader15693(void);//处理接收到的数据到新的数组，并重新接收
void UartAction_Reader15693(uint8_t *buf,uint8_t len);//串口接收函数
uint8_t UartRead_Reader15693(uint8_t *buf,uint8_t len);//串口数据读取处理
void UartRxdMonitor_Reader15693(uint8_t time_ms);//串口超时接收监控函数
void UartDriver_Reader15693(void);//串口主循环驱动函数
void UartWrite_Add_MSB_CRC16_Reader15693(uint8_t *buf, uint16_t len);

#define Cmd_GetReaderInformation_Reader15693 0x00
uint16_t Get_MSB_CRC16(uint8_t *buf,uint8_t len);

void Set_BookLabel_Reader15693(uint8_t IsMCUInformation,uint8_t IsReaderInformation,\
								uint16_t x,uint16_t y,\
								uint8_t rssi,uint8_t *uid);
#endif /* INC_READER15693_H_ */
