/*
 * Reader15693.c
 *
 *  Created on: Oct 16, 2024
 *      Author: 45951
 */

#include "Reader15693.h"
uint8_t Flag_Events_Reader15693 = 0;
uint8_t	Flag_Event_Init_Reader15693 = 0;
uint8_t Rxdbuf_Reader15693[1] = {0};
uint8_t CntRxd_Reader15693 = 0;
uint8_t UartRxd_Reader15693[255] = {0};
uint8_t Flag_UartRxdMonitor_Reader15693 = 0;
uint8_t UartSend_Reader15693[255] = {0};

uint16_t x_CurrentPosition_Reader15693 = 0;
uint16_t y_CurrentPosition_Reader15693 = 0;
uint8_t Is_x_PositionCorrected = 0;
uint8_t Is_y_PositionCorrected = 0;
uint16_t Number_SavedBookLabels = 0;
struct RFID
{
	uint8_t UID[8];
	uint8_t RSSI;
	uint8_t	AFI;
	uint16_t x_Distance;
	uint16_t y_Distance;
};
struct RFID myRFID[1000];//全局变量自动赋值0
uint8_t GetStatus_Number_SavedBookLabels_Reader15693(void)
{
	return Number_SavedBookLabels;
}
void SetStatus_Number_SavedBookLabels_Reader15693(int8_t ChangeNum,uint8_t CleanFlag)
{
	if(CleanFlag)
	{
		Number_SavedBookLabels = 0;
	}
	else
	{
		Number_SavedBookLabels += ChangeNum;
	}
}
uint8_t GetStatus_Is_x_PositionCorrected_Reader15693(void)
{
	return Is_x_PositionCorrected;
}
void SetStatus_Is_x_PositionCorrected_Reader15693(uint8_t status)
{
	Is_x_PositionCorrected = status;
}
uint8_t GetStatus_Is_y_PositionCorrected_Reader15693(void)
{
	return Is_y_PositionCorrected;
}
void SetStatus_Is_y_PositionCorrected_Reader15693(uint8_t status)
{
	Is_y_PositionCorrected = status;
}
void Set_BookLabel_Reader15693(uint8_t IsMCUInformation,uint8_t IsReaderInformation,\
								uint16_t x,uint16_t y,\
								uint8_t rssi,uint8_t *uid)
{
	if(IsMCUInformation)
	{
		x_CurrentPosition_Reader15693 = x;
		y_CurrentPosition_Reader15693 = y;
	}
	if(IsReaderInformation)
	{
		uint8_t Flag_cmp = 1;
		for(uint16_t i=0;i<Number_SavedBookLabels;i++)
		{
			Flag_cmp = memcmp(uid , myRFID[i].UID,8);
			if(Flag_cmp == 0)
			{//重复
				break;
			}
		}
		if(Flag_cmp)
		{//不重复
			for(uint8_t j=0;j<8;j++)
			{
				myRFID[Number_SavedBookLabels].UID[j] = uid[j];
			}
			myRFID[Number_SavedBookLabels].RSSI = rssi;
			myRFID[Number_SavedBookLabels].x_Distance = x_CurrentPosition_Reader15693;
			myRFID[Number_SavedBookLabels].y_Distance = y_CurrentPosition_Reader15693;
			SetStatus_Number_SavedBookLabels_Reader15693(1,KeepBookLabels);
		}
		else
		{//重复

		}
	}
}
void Event_Inventory_Reader15693(void)
{

}
void UartAction_Reader15693(uint8_t *buf,uint8_t len)
{
	UartWrite_WirelessModule(buf, len);
	return;
	switch(buf[2])
	{
		case Cmd_GetReaderInformation_Reader15693:
		{
			Flag_Event_Init_Reader15693 = 3;
			return;
		}break;
	}
}
void Events_Reader15693(void)
{
	UartDriver_Reader15693();
	switch(Flag_Events_Reader15693)
	{
		case 0:
		{
			Event_Init_Reader15693();
		}break;
		case 1:
		{
			Event_Inventory_Reader15693();
		}break;
	}
}
void Event_Init_Reader15693(void)
{
	switch(Flag_Event_Init_Reader15693)
	{
		case 0:
		{
			UartReceiveIT_Reader15693();//设置单片机的各个关于读写器的引脚“通讯”、“电源”
			Flag_Event_Init_Reader15693 = 4;
//			UartWrite_WirelessModule(myRFID[998].UID, 8);
//			myRFID[900].UID[0] = 0x12;
//			myRFID[900].UID[1] = 0x34;
//			myRFID[900].UID[2] = 0x56;
//			myRFID[900].UID[3] = 0x78;
//			myRFID[900].UID[4] = 0x12;
//			myRFID[900].UID[5] = 0x34;
//			myRFID[900].UID[6] = 0x56;
//			myRFID[900].UID[7] = 0x78;
//			uint8_t testflag = 0;
//			uint16_t test_i = 0;
//			for(uint16_t i = 0;i<1000;i++)
//			{
//				testflag = memcmp(myRFID[900].UID,myRFID[i].UID,8);
//				if(!testflag)
//				{
//
//					break;
//				}
//			}
//			myRFID[998].UID[0] = test_i >> 8;
//			myRFID[998].UID[1] = test_i & 0xFF;
//			UartWrite_WirelessModule(myRFID[900].UID, 8);
		}break;
		case 1:
		{
			//定时器定时变2
		}break;
		case 2:
		{
			//串口发送检测指令UartInit_Reader15693();
			Flag_Event_Init_Reader15693 = 3;

		}break;
		case 3:
		{
			//等待串口返回，返回赋值4，超时定时器赋值5

			Flag_Event_Init_Reader15693 = 4;
		}break;
		case 4:
		{
			Flag_Events_Reader15693 = 1;
		}break;
		case 5:
		{
			Flag_Event_Init_Reader15693 = 1;
		}break;
	}
}
void TimerTasks_Reader15693(void)
{
	UartRxdMonitor_Reader15693(1);
	uint8_t TimerFlag_InitFlag_Events_Reader15693 = 0;
	switch(Flag_Event_Init_Reader15693)
	{
		case 1:
		{
			TimerFlag_InitFlag_Events_Reader15693++;
			if(TimerFlag_InitFlag_Events_Reader15693 >= 2000)
			{
				TimerFlag_InitFlag_Events_Reader15693 = 0;
				Flag_Event_Init_Reader15693 = 2;
			}
		}break;
		case 3:
		{
			TimerFlag_InitFlag_Events_Reader15693++;
			if(TimerFlag_InitFlag_Events_Reader15693 >= 1000)
			{
				TimerFlag_InitFlag_Events_Reader15693 = 0;
				Flag_Event_Init_Reader15693 = 5;
			}
		}break;
	}
}
void UartWrite_Cmd_Reader15693(uint8_t Cmd)
{
	switch(Cmd)
	{
		case Cmd_GetReaderInformation_Reader15693:
		{
			uint8_t uartsendbuf[4] = {0x05,0xFF,Cmd_GetReaderInformation_Reader15693,0xF0};
			UartWrite_Add_MSB_CRC16_Reader15693(uartsendbuf,4);
		}break;

	}
}

void UartReceiveIT_Reader15693(void)
{//读写器串口接收
	HAL_UART_Receive_IT(&huart_Reader15693, (uint8_t *)Rxdbuf_Reader15693, 1);// 重新启动接收中断
}
void UartGetData_Reader15693(void)
{//处理接收到的数据到新的数组，并重新接收
	static uint8_t remain_len = 0;
	UartReceiveIT_Reader15693();
	UartRxd_Reader15693[CntRxd_Reader15693] = Rxdbuf_Reader15693[0];
	//05 01 86 90 95
	if(CntRxd_Reader15693 == 0)
	{//len
		remain_len = UartRxd_Reader15693[0];
		if(remain_len<4)
		{//数据长度4-255
			CntRxd_Reader15693 = 0;
		}
		else
		{
			remain_len = UartRxd_Reader15693[0];
			CntRxd_Reader15693++;
			remain_len--;
		}
	}
	else
	{
		CntRxd_Reader15693++;
		remain_len--;
	}
	if(CntRxd_Reader15693 > 0 && remain_len == 0)
	{
		Flag_UartRxdMonitor_Reader15693 = 1;
	}
}
void UartWrite_Reader15693(uint8_t *pData, uint16_t Size)
{//发送给读写器
	HAL_UART_Transmit(&huart_Reader15693,pData,Size,1000);
}
uint8_t UartRead_Reader15693(uint8_t *buf,uint8_t len)
{
	uint8_t i;
	if(len > CntRxd_Reader15693)
	{
		len = CntRxd_Reader15693;
	}
	for(i = 0;i < len;i++)
	{
		buf[i] = UartRxd_Reader15693[i];
	}
	CntRxd_Reader15693 = 0;
	return len;
}
void UartRxdMonitor_Reader15693(uint8_t time_ms)
{
	static uint8_t cnt_bkp = 0;
	static uint8_t idle_timer = 0;
	if(CntRxd_Reader15693>0)
	{
		if(cnt_bkp != CntRxd_Reader15693)
		{
			cnt_bkp = CntRxd_Reader15693;
			idle_timer = 0;
		}
		else
		{
			if(idle_timer<10)
			{
				idle_timer += time_ms;
				if(idle_timer >=10)
				{
					Flag_UartRxdMonitor_Reader15693 = 1;
				}
			}
		}
	}
	else
	{
		cnt_bkp = 0;
	}
}
void UartDriver_Reader15693(void)
{
	uint8_t len;
	uint8_t buf[255] = {0};
	if(Flag_UartRxdMonitor_Reader15693)
	{
		Flag_UartRxdMonitor_Reader15693 = 0;
		len = UartRead_Reader15693(buf,sizeof(buf));
		UartAction_Reader15693(buf, len);
	}
}
uint16_t Get_MSB_CRC16(uint8_t *buf,uint8_t len)
{
	uint16_t crc = 0xFFFF;
	uint16_t i,j;
	for(i=0;i<len;i++)
	{
		crc ^= (uint16_t)buf[i];
		for(j=0;j<8;j++)
		{
			crc= crc&0x0001 ? (crc>>1)^0x8408 : crc >> 1;
		}
	}
	return crc;
}
void UartWrite_Add_MSB_CRC16_Reader15693(uint8_t *buf, uint16_t len)
{//串口发送函数
	uint16_t crc = Get_MSB_CRC16(buf,len);
	for (uint8_t i = 0; i < len; i++)
	{
		UartSend_Reader15693[i] = buf[i];
	}
	len += 2;
	UartSend_Reader15693[len-2] = crc & 0xFF;//低位在前
	UartSend_Reader15693[len-1] = crc >> 8;//高位在后
	UartWrite_Reader15693(UartSend_Reader15693,len);
}

