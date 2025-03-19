#pragma once

#include "usart.h"

#define RECV_BUF_LEN 256

void upper_uart_reg_handle(UART_HandleTypeDef *h_uart, DMA_HandleTypeDef *h_dma);

void upper_uart_init(void);

UART_HandleTypeDef *GetUpperUartHandle(void);
void                upper_receive_rx_data(void);