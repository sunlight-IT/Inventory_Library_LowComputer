/*
 * MainLogic.c
 *
 *  Created on: Jun 4, 2024
 *      Author: 45951
 */

#include "MainLogic.h"

uint8_t Flag_Events_MainLogic = 0;
void    Events_MainLogic(void) {
  switch (Flag_Events_MainLogic) {
    case 0: {
      HAL_TIM_Base_Start_IT(&htim1);
      HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
      Flag_Events_MainLogic = 1;
    } break;
    case 1: {
      // Events_Reader15693();//璇诲啓鍣ㄤ簨浠�
      // Events_WirelessModule();//鏃犵嚎涓插彛浜嬩欢
      // Events_BLDCMotor();
      Events_ServoMotor();
    } break;
  }
}

typedef struct {
  uint16_t Cnt_Rxd;
  uint8_t  Buffer_Rxd;
  uint8_t  Flag_Packet_Received;
} UartVariate;

typedef struct {
  UartVariate *uartvariate;
  void (*Uart_Driver)(void);
  uint8_t (*Uart_Read)(uint8_t *buf, uint8_t len);
  void (*UartAction)(uint8_t *buf, uint8_t len);
} UartFunctions;

void Uart_Driver1(void) {}
void Uart_Driver2(void) {
  UartFunctions test1;
  test1.Uart_Driver = Uart_Driver1;
}
