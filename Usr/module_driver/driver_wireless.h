#pragma once

#include "dma.h"
#include "usart.h"

#define WIRELESS_MAX_BUF_LEN 256

// typedef enum {
//   kWirelessStop  = 0x00,
//   kWirelessStart = 0x01,
//   kBook          = 0x02,
//   kWirelessIdle  = 0xff,
// } ENUM_CMD;

typedef struct {
  uint8_t j;
  uint8_t i;
  uint8_t l;
} RecvPackHead_t;

UART_HandleTypeDef *GetWireLessHandle(void);
void                receive_rx_data(void);

void wireless_reg_handle(UART_HandleTypeDef *h_uart, DMA_HandleTypeDef *h_dma);
void wireless_init(void);

void wireless_send(uint8_t *data, uint8_t len);
