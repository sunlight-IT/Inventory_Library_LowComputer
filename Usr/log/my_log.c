#include "my_log.h"

#include <stdarg.h>

#include "usart.h"

#include <stdio.h>
// #ifdef __GNUC__
// #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
// #else
// #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
// #endif

// PUTCHAR_PROTOTYPE
// {
//   HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0x200);
//   return ch;
// }

// �ض��� printf
int __io_putchar(int ch)
{
  HAL_UART_Transmit(&huart5, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}
