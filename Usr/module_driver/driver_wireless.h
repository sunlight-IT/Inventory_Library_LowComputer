#pragma once

#include "usart.h"
#include "dma.h"

#define WIRELESS_MAX_BUF_LEN 128

typedef struct
{
    uint8_t j;
    uint8_t i;
    uint8_t l;
} RecvPackHead_t;

UART_HandleTypeDef *
GetWireLessHandle(void);
void receive_rx_data(void);

void wireless_reg_handle(UART_HandleTypeDef *h_uart, DMA_HandleTypeDef *h_dma);
void wireless_init(void);

void wireless_send(uint8_t *data, uint8_t len);
void wireless_receive(void);

uint8_t GetCmdType(void);