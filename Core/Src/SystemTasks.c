/*
 * SystemTasks.c
 *
 *  Created on: Oct 21, 2024
 *      Author: 45951
 */

#include "SystemTasks.h"
#include "module_driver/driver_wireless.h"
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// if (huart->Instance == UartX_Reader15693)
	// {
	// 	UartGetData_Reader15693(); // 处理接收到的数据
	// }
	// if (huart->Instance == UartX_WirelessModule)
	// {
	// 	UartGetData_WirelessModule(); // 处理接收到的数据
	// }
	if (huart->Instance == UartX_ServoMotor)
	{
		UartGetData_ServoMotor(); // 处理接收到的数据
	}
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart == GetWireLessHandle())
	{
		receive_rx_data();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM1) // 1ms
	{
		// TimerTasks_WirelessModule();
		// TimerTasks_Reader15693();
		TimerTasks_ServoMotor();
	}
}
