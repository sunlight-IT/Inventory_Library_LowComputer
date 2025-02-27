/*
 * Template.h
 *
 *  Created on: Oct 22, 2024
 *      Author: 45951
 */

#ifndef INC_TEMPLATE_H_
#define INC_TEMPLATE_H_
#include "main.h"

#define huart_Template 	huart1
#define UartX_Template	USART1
void Events_Template(void);//读写器事件
void Event_Init_Template(void);//初始化事件
void TimerTasks_Template(void);
//串口
void UartWrite_Template(uint8_t *pData, uint16_t Size);//串口发送函数
void UartReceiveIT_Template(void);//读写器串口中断接收
void UartGetData_Template(void);//处理接收到的数据到新的数组，并重新接收
void UartAction_Template(uint8_t *buf,uint8_t len);//串口接收函数
uint8_t UartRead_Template(uint8_t *buf,uint8_t len);//串口数据读取处理
void UartRxdMonitor_Template(uint8_t time_ms);//串口超时接收监控函数
void UartDriver_Template(void);//串口主循环驱动函数
void UartWrite_Add_MSB_CRC16_Template(uint8_t *buf, uint16_t len);

#define Cmd_GetReaderInformation_Template 0x00


#endif /* INC_TEMPLATE_H_ */
