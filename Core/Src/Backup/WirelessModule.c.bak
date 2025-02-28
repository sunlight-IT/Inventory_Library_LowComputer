/*
 * WirelessModule.c
 *
 *  Created on: Oct 21, 2024
 *      Author: 45951
 */

#include "WirelessModule.h"

uint8_t Flag_Events_WirelessModule = 0;
uint8_t	Flag_Event_Init_WirelessModule = 0;
uint8_t Rxdbuf_WirelessModule[1] = {0};
uint8_t CntRxd_WirelessModule = 0;
uint8_t UartRxd_WirelessModule[255] = {0};
uint8_t Flag_UartRxdMonitor_WirelessModule = 0;
uint8_t UartSend_WirelessModule[255] = {0};
void Events_WirelessModule(void)
{
	UartDriver_WirelessModule();
	switch(Flag_Events_WirelessModule)
	{
		case 0:
		{
			Event_Init_WirelessModule();
		}break;
		case 1:
		{

		}break;
	}
}
void Event_Init_WirelessModule(void)
{
	switch(Flag_Event_Init_WirelessModule)
	{
		case 0:
		{
			UartReceiveIT_WirelessModule();//设置单片机的各个关于读写器的引脚“通讯”、“电源”
			Flag_Event_Init_WirelessModule = 4;
		}break;
		case 1:
		{
			//定时器定时变2
		}break;
		case 2:
		{
			//串口发送检测指令UartInit_WirelessModule();
			Flag_Event_Init_WirelessModule = 3;
		}break;
		case 3:
		{
			//等待串口返回，返回赋值3，超时定时器赋值4


			Flag_Event_Init_WirelessModule = 4;
		}break;
		case 4:
		{
			Flag_Events_WirelessModule = 1;
		}break;
		case 5:
		{
			Flag_Event_Init_WirelessModule = 1;
		}break;
	}
}
void TimerTasks_WirelessModule(void)
{
	UartRxdMonitor_WirelessModule(1);
}

void UartWrite_Cmd_WirelessModule(uint8_t Cmd)
{
	switch(Cmd)
	{
		case 0:
		{

		}break;

	}
}

void UartAction_WirelessModule(uint8_t *buf,uint8_t len)
{
	uint8_t uartsendbuf[8] = {0x00,0xA0,0x00,0x00,0x00,0x5E,0x00,0x00};
	UartWrite_Reader15693(buf, len);
	UartWrite_WirelessModule(buf, len);
	return;
	if(buf[0] == 0x00)
	{
		Uart_Send_MovementRegister_ServoMotor(3,((uint16_t)buf[1]<<8 | buf[2]));
	}
	if(buf[0] == 0x01)
		{
			Uart_Send_MovementRegister_ServoMotor(1,((uint16_t)buf[1]<<8 | buf[2]));
		}
	if(buf[0] == 0x02)
		{
			Uart_Send_MovementRegister_ServoMotor(2,((uint16_t)buf[1]<<8 | buf[2]));
		}
	if(buf[0] == 0x03)
	{
		test(buf);
		UartWrite_WirelessModule(buf, len);
	}
//	UartWrite_Add_MODBUS_CRC16_WirelessModule(buf,len);
//	switch(buf[2])
//	{
//		case 0:
//		{
//			Flag_Event_Init_WirelessModule = 3;
//			return;
//		}break;
//	}
}
void UartReceiveIT_WirelessModule(void)
{//读写器串口接收
	HAL_UART_Receive_IT(&huart_WirelessModule, (uint8_t *)Rxdbuf_WirelessModule, 1);// 重新启动接收中断
}
void UartGetData_WirelessModule(void)
{//处理接收到的数据到新的数组，并重新接收
	UartRxd_WirelessModule[CntRxd_WirelessModule] = Rxdbuf_WirelessModule[0];
	CntRxd_WirelessModule++;
	UartReceiveIT_WirelessModule();
}
void UartWrite_WirelessModule(uint8_t *pData, uint16_t Size)
{//发送给读写器
	HAL_UART_Transmit(&huart_WirelessModule,pData,Size,1000);
}
uint8_t UartRead_WirelessModule(uint8_t *buf,uint8_t len)
{
	uint8_t i;
	if(len > CntRxd_WirelessModule)
	{
		len = CntRxd_WirelessModule;
	}
	for(i = 0;i < len;i++)
	{
		buf[i] = UartRxd_WirelessModule[i];
	}
	CntRxd_WirelessModule = 0;
	return len;
}
void UartRxdMonitor_WirelessModule(uint8_t time_ms)
{
	static uint8_t cnt_bkp = 0;
	static uint8_t idle_timer = 0;
	if(CntRxd_WirelessModule>0)
	{
		if(cnt_bkp != CntRxd_WirelessModule)
		{
			cnt_bkp = CntRxd_WirelessModule;
			idle_timer = 0;
		}
		else
		{
			if(idle_timer<10)
			{
				idle_timer += time_ms;
				if(idle_timer >=10)
				{
					Flag_UartRxdMonitor_WirelessModule = 1;
				}
			}
		}
	}
	else
	{
		cnt_bkp = 0;
	}
}
void UartDriver_WirelessModule(void)
{
	uint8_t len;
	uint8_t buf[255] = {0};
	if(Flag_UartRxdMonitor_WirelessModule)
	{
		Flag_UartRxdMonitor_WirelessModule = 0;
		len = UartRead_WirelessModule(buf,sizeof(buf));
		UartAction_WirelessModule(buf, len);
	}
}
void Get_MODBUSCRC16_WirelessModule(uint8_t *buf,uint16_t len,uint8_t *crcl,uint8_t *crch)
{
	uint16_t crc = 0xFFFF;
	for (uint16_t i = 0; i < len; i++)
	{
		crc ^= buf[i]; // 将数据字节与CRC的高字节异或
		for (uint16_t j = 0; j < 8; j++) {
			if (crc & 1)
			{ // 如果最低位为1
				crc = (crc >> 1) ^ 0xA001; // 右移1位，并与多项式异或
			}
			else
			{
				crc >>= 1; // 右移1位
			}
		}
	}
	*crcl = crc & 0xFF;//对外赋值
	*crch = crc >> 8;//对外赋值
}
void UartWrite_Add_MODBUS_CRC16_WirelessModule(uint8_t *buf, uint16_t len)
{//串口发送函数
	uint8_t UartSend_WirelessModule[255] = {0};
	for (uint8_t i = 0; i < len; i++)
	{
		UartSend_WirelessModule[i] = buf[i];
	}
	Get_MODBUSCRC16_WirelessModule(UartSend_WirelessModule,len,&UartSend_WirelessModule[len],&UartSend_WirelessModule[len+1]);
	UartWrite_WirelessModule(UartSend_WirelessModule,len+2);
}
