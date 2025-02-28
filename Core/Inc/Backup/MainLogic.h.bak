/*
 * MainLogic.h
 *
 *  Created on: Jun 4, 2024
 *      Author: 45951
 */

#ifndef INC_MAINLOGIC_H_
#define INC_MAINLOGIC_H_
#include "main.h"
struct Event
{
	uint8_t Flag_Event[4];
	uint16_t Flag_Timer[4];
	uint8_t Uart_RxdByte[1];
	uint8_t Uart_RxdCnt;
	uint8_t Uart_Rxd[255];
	uint8_t Uart_Flag_RxdMonitor;
	uint8_t Uart_Send[255];
};

void Events_MainLogic(void);
#endif /* INC_MAINLOGIC_H_ */
