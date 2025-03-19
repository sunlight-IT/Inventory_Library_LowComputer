/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Template.h"

#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"
#include <stdbool.h>
#include <stdint.h>
#include "BLDCMotor.h"
#include "MainLogic.h"
#include "Reader15693.h"
#include "WirelessModule.h"

#include "ServoMotor.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Power_EStopLED_Pin GPIO_PIN_13
#define Power_EStopLED_GPIO_Port GPIOC
#define Power_Sensor_Pin GPIO_PIN_0
#define Power_Sensor_GPIO_Port GPIOC
#define Input_Sensor3_Pin GPIO_PIN_1
#define Input_Sensor3_GPIO_Port GPIOC
#define DebugSerial_TX_Pin GPIO_PIN_2
#define DebugSerial_TX_GPIO_Port GPIOA
#define DebugSerial_RX_Pin GPIO_PIN_3
#define DebugSerial_RX_GPIO_Port GPIOA
#define ADC_ContactPoint_Pin GPIO_PIN_4
#define ADC_ContactPoint_GPIO_Port GPIOA
#define ADC_BAT_Pin GPIO_PIN_5
#define ADC_BAT_GPIO_Port GPIOA
#define ADC_DetectionSwitch_Pin GPIO_PIN_6
#define ADC_DetectionSwitch_GPIO_Port GPIOA
#define ADC_CurrentDetection_Pin GPIO_PIN_7
#define ADC_CurrentDetection_GPIO_Port GPIOA
#define Input_Sensor1_Pin GPIO_PIN_6
#define Input_Sensor1_GPIO_Port GPIOC
#define Input_Sensor2_Pin GPIO_PIN_7
#define Input_Sensor2_GPIO_Port GPIOC
#define Input_EmergencyStopButton_Pin GPIO_PIN_8
#define Input_EmergencyStopButton_GPIO_Port GPIOC
#define Power_Host_Pin GPIO_PIN_9
#define Power_Host_GPIO_Port GPIOC
#define Power_BLDCMotor_Pin GPIO_PIN_8
#define Power_BLDCMotor_GPIO_Port GPIOA
#define AndroidSerial_TX_Pin GPIO_PIN_9
#define AndroidSerial_TX_GPIO_Port GPIOA
#define AndroidSerial_RX_Pin GPIO_PIN_10
#define AndroidSerial_RX_GPIO_Port GPIOA
#define Power_WirelessModule_Pin GPIO_PIN_11
#define Power_WirelessModule_GPIO_Port GPIOA
#define Power_Reader15693_Pin GPIO_PIN_12
#define Power_Reader15693_GPIO_Port GPIOA
#define Power_Lidar2_Pin GPIO_PIN_15
#define Power_Lidar2_GPIO_Port GPIOA
#define Power_Lidar1_Pin GPIO_PIN_3
#define Power_Lidar1_GPIO_Port GPIOB
#define Direction_Ctl_BLDCMotor_Pin GPIO_PIN_4
#define Direction_Ctl_BLDCMotor_GPIO_Port GPIOB
#define Speed_PWMCtl_BLDCMotor_Pin GPIO_PIN_5
#define Speed_PWMCtl_BLDCMotor_GPIO_Port GPIOB
#define SCL_OLED_Pin GPIO_PIN_8
#define SCL_OLED_GPIO_Port GPIOB
#define SDA_OLED_Pin GPIO_PIN_9
#define SDA_OLED_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
