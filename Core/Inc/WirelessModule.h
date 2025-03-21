/*
 * WirelessModule.h
 *
 *  Created on: Oct 21, 2024
 *      Author: 45951
 */

#ifndef INC_WIRELESSMODULE_H_
#define INC_WIRELESSMODULE_H_
#include "main.h"
#define huart_WirelessModule huart4
#define UartX_WirelessModule USART1
void Events_WirelessModule(void);      // 读写器事件
void Event_Init_WirelessModule(void);  // 初始化事件
void TimerTasks_WirelessModule(void);
// 串口
void    UartWrite_WirelessModule(uint8_t *pData, uint16_t Size);  // 串口发送函数
void    UartReceiveIT_WirelessModule(void);                       // 读写器串口中断接收
void    UartGetData_WirelessModule(void);                         // 处理接收到的数据到新的数组，并重新接收
void    UartAction_WirelessModule(uint8_t *buf, uint8_t len);     // 串口接收函数
uint8_t UartRead_WirelessModule(uint8_t *buf, uint8_t len);       // 串口数据读取处理
void    UartRxdMonitor_WirelessModule(uint8_t time_ms);           // 串口超时接收监控函数
void    UartDriver_WirelessModule(void);                          // 串口主循环驱动函数
void    UartWrite_Add_MODBUS_CRC16_WirelessModule(uint8_t *buf, uint16_t len);

void Get_MODBUSCRC16_WirelessModule(uint8_t *buf, uint16_t len, uint8_t *crcl, uint8_t *crch);
#endif /* INC_WIRELESSMODULE_H_ */
