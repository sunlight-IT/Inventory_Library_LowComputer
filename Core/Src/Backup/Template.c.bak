/*
 * Template.c
 *
 *  Created on: Oct 22, 2024
 *      Author: 45951
 */

#include "Template.h"

uint8_t Flag_Events_Template = 0;
uint8_t	Flag_Event_Init_Template = 0;
uint8_t Rxdbuf_Template[1] = {0};
uint8_t CntRxd_Template = 0;
uint8_t UartRxd_Template[40] = {0};
uint8_t Flag_UartRxdMonitor_Template = 0;
uint8_t UartSend_Template[40] = {0};

void UartAction_Template(uint8_t *buf,uint8_t len)
{
	Flag_Event_Init_Template = 4;
	UartWrite_WirelessModule(buf, len);
	switch(buf[2])
	{
		case Cmd_GetReaderInformation_Template:
		{
			Flag_Event_Init_Template = 3;
			return;
		}break;
	}
}
void Events_Template(void)
{
	UartDriver_Template();
	switch(Flag_Events_Template)
	{
		case 0:
		{
			Event_Init_Template();
		}break;
		case 1:
		{

		}break;
	}
}
void Event_Init_Template(void)
{
	switch(Flag_Event_Init_Template)
	{
		case 0:
		{//模块初始化配置写在这个位置
			UartReceiveIT_Template();//设置单片机的各个关于读写器的引脚“通讯”、“电源”
			Flag_Event_Init_Template = 1;
		}break;
		case 1:
		{
			//定时器定时变2
		}break;
		case 2:
		{
			//串口发送检测指令UartInit_Template();
			Flag_Event_Init_Template = 3;
		}break;
		case 3:
		{
			//等待串口返回，返回赋值4，超时定时器赋值5
		}break;
		case 4:
		{//退出初始化
			Flag_Events_Template = 1;
		}break;
		case 5:
		{//重新发送一次
			Flag_Event_Init_Template = 1;
		}break;
	}
}
void TimerTasks_Template(void)
{
	UartRxdMonitor_Template(1);
	static uint16_t TimerFlag_InitFlag_Events_Template = 0;
	switch(Flag_Event_Init_Template)
	{
		case 1:
		{
			TimerFlag_InitFlag_Events_Template++;
			if(TimerFlag_InitFlag_Events_Template >= 2000)
			{
				TimerFlag_InitFlag_Events_Template = 0;
				Flag_Event_Init_Template = 2;
			}
		}break;
		case 3:
		{
			TimerFlag_InitFlag_Events_Template++;
			if(TimerFlag_InitFlag_Events_Template >= 1000)
			{
				TimerFlag_InitFlag_Events_Template = 0;
				Flag_Event_Init_Template = 5;
			}
		}break;
	}
}
void UartWrite_Cmd_Template(uint8_t Cmd)
{
	switch(Cmd)
	{
		case Cmd_GetReaderInformation_Template:
		{
			uint8_t uartsendbuf[4] = {0x05,0xFF,Cmd_GetReaderInformation_Template,0xF0};
			UartWrite_Add_MSB_CRC16_Template(uartsendbuf,4);
		}break;

	}
}

void UartReceiveIT_Template(void)
{//读写器串口接收
	HAL_UART_Receive_IT(&huart_Template, (uint8_t *)Rxdbuf_Template, 1);// 重新启动接收中断
}
void UartGetData_Template(void)
{//处理接收到的数据到新的数组，并重新接收
	UartRxd_Template[CntRxd_Template] = Rxdbuf_Template[0];
	CntRxd_Template++;
	UartReceiveIT_Template();
}
void UartWrite_Template(uint8_t *pData, uint16_t Size)
{//发送给读写器
	HAL_UART_Transmit(&huart_Template,pData,Size,1000);
}
uint8_t UartRead_Template(uint8_t *buf,uint8_t len)
{
	uint8_t i;
	if(len > CntRxd_Template)
	{
		len = CntRxd_Template;
	}
	for(i = 0;i < len;i++)
	{
		buf[i] = UartRxd_Template[i];
	}
	CntRxd_Template = 0;
	return len;
}
void UartRxdMonitor_Template(uint8_t time_ms)
{
	static uint8_t cnt_bkp = 0;
	static uint8_t idle_timer = 0;
	if(CntRxd_Template>0)
	{
		if(cnt_bkp != CntRxd_Template)
		{
			cnt_bkp = CntRxd_Template;
			idle_timer = 0;
		}
		else
		{
			if(idle_timer<10)
			{
				idle_timer += time_ms;
				if(idle_timer >=10)
				{
					Flag_UartRxdMonitor_Template = 1;
				}
			}
		}
	}
	else
	{
		cnt_bkp = 0;
	}
}
void UartDriver_Template(void)
{
	uint8_t len;
	uint8_t buf[40] = {0};
	if(Flag_UartRxdMonitor_Template)
	{
		Flag_UartRxdMonitor_Template = 0;
		len = UartRead_Template(buf,sizeof(buf));
		UartAction_Template(buf, len);
	}
}

void UartWrite_Add_MSB_CRC16_Template(uint8_t *buf, uint16_t len)
{//串口发送函数
	uint16_t crc = Get_MSB_CRC16(buf,len);
	for (uint8_t i = 0; i < len; i++)
	{
		UartSend_Template[i] = buf[i];
	}
	len += 2;
	UartSend_Template[len-2] = crc & 0xFF;//低位在前
	UartSend_Template[len-1] = crc >> 8;//高位在后
	UartWrite_Template(UartSend_Template,len);
}



