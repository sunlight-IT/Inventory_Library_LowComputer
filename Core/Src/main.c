/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "component/cmd_process/cmd_process.h"
#include "component/endianness_change.h"
#include "log/my_log.h"
#include "module_driver/driver_motor.h"
#include "module_driver/driver_upp_uart.h"
#include "module_driver/driver_wireless.h"
#include "module_middle/middle_event_process.h"
#include "tick/tick.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
typedef enum {
  kWirelessStop  = 0x00,
  kWirelessStart = 0x01,
  kBook          = 0x02,
  kWirelessIdle  = 0xff,
} ENUM_CMD;

uint32_t cmd;

static uint8_t  pandian_flag;
static uint32_t pandian_time;

static uint8_t data_book;
static uint8_t flag;

static ENUM_CMD move_state      = kWirelessIdle;
static uint32_t distance_target = 300;

static uint8_t  crc_cal[] = {0xd0, 0x20, 0x01, 0x0a, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0xA1, 0xd1};
uint16_t        test_1    = 0x1234;
uint32_t        test_2    = 0x12345678;
static uint16_t CRC16_Calculate(uint8_t *data, uint8_t len) {
  uint16_t crc16 = 0xffff;
  uint16_t temp  = 0;
  for (int i = 0; i < len; i++) {
    crc16 ^= (uint16_t)data[i];
    for (int j = 0; j < 8; j++) {
      if (crc16 & 0x0001) {
        crc16 = (crc16 >> 1) ^ 0xa001;
      } else {
        crc16 = (crc16 >> 1);
      }
    }
  }
  LOGI("CRC is :%04x", crc16);
  return crc16;
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_UART4_Init();
  MX_UART5_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM4_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  wireless_reg_handle(&huart1, &hdma_usart1_rx);
  wireless_init();

  upper_uart_reg_handle(&huart4, &hdma_uart4_rx);
  upper_uart_init();

  motor_uart_init();

  servo_config_init();

  app_communicate_init();
  app_servo_action_init();
  // servo_info_updata(KGoRight, 30);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    process_event();

    Events_MainLogic();

    app_comunicate();
    app_action();
    // if (GetMoterFlag() >= 22) {
    //   app_comunicate();
    //   if ((get_action_mode() == 0x01) && !pandian_flag) {
    //     LOGE("TEST");
    //     switch (move_state) {
    //       case kWirelessIdle:
    //         move_state = kWirelessStart;
    //         break;
    //       case kWirelessStart:
    //         if (servo_move(KGoRight)) {
    //           servo_info_updata(KGoRight, 30);
    //           pandian_flag = 1;
    //         }
    //         break;
    //     }
    //   }

    //   switch (pandian_flag) {
    //     case 1:
    //       uint16_t pos1    = get_Position_mm(GetMoterStateReg(), 4);
    //       uint16_t pos_tar = get_servo_config_pos();
    //       uint8_t  tmp     = pos_tar;
    //       // pos_tar >>= 8;
    //       // pos_tar |= ((uint16_t)tmp) << 8;
    //       LOGI("pos_tar3 is :%04x", pos_tar);
    //       LOGI("cur pos is %04x", pos1);
    //       if (pos1 >= pos_tar - 10 && pos1 <= pos_tar + 100 && pos1 != 0xff) {
    //         pandian_flag = 2;
    //         LOGI("check finish %d", pandian_flag);
    //       }
    //       break;
    //     case 2:
    //       if (servo_speed_set(0)) {
    //         pandian_flag = 25;
    //         LOGI("Back %d ", pandian_flag);
    //       }

    //       break;
    //     case 3:
    //       if (servo_move(KGoLeft)) {
    //         servo_info_updata(KGoLeft, 30);
    //         pandian_flag = 4;

    //         LOGI("Back move %d", pandian_flag);
    //       }

    //       break;
    //     case 4:
    //       uint16_t pos = get_Position_mm(GetMoterStateReg(), 4);
    //       if (pos <= distance_target - 100 && pos != 0) {
    //         pandian_flag = 5;
    //         LOGI("back finish %d", pandian_flag);
    //       }
    //       break;
    //     case 5:
    //       if (servo_speed_set(0)) {
    //         pandian_flag = 6;
    //         LOGI("OK %d ", pandian_flag);
    //       }

    //       break;
    //     default:

    //       break;
    // }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // }
    /* USER CODE END 3 */
  }
}
/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef       RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef       RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit     = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection    = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
