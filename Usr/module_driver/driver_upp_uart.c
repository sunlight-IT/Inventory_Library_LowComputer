#include "driver_upp_uart.h"

// #include "component/cmd_process/cmd_process.h"
#include "log/my_log.h"
#include "module_apply/app_communicate.h"
#include "module_middle/middle_event_process.h"
#include "module_middle/middle_fsm.h"

static UART_HandleTypeDef *m_uart;
static DMA_HandleTypeDef  *m_dma;

static uint8_t rx_buf[RECV_BUF_LEN];
static uint8_t rx_len;

static uint8_t rx_cache;
static bool    flag = false;

static void upper_packet_analys(void);

void upper_uart_reg_handle(UART_HandleTypeDef *h_uart, DMA_HandleTypeDef *h_dma) {
  m_uart = h_uart;
  m_dma  = h_dma;
}

void upper_uart_init(void) {
  registCallback(EVENT_UpperUart, upper_packet_analys);          // 注册事件表
  HAL_UARTEx_ReceiveToIdle_DMA(m_uart, rx_buf, sizeof(rx_buf));  // 串口�?�?+dma
  __HAL_DMA_DISABLE_IT(m_dma, DMA_IT_HT);                        // 关闭dma接收半满�?�?函数
}

UART_HandleTypeDef *GetUpperUartHandle(void) {
  if (m_uart == NULL) {
    LOGW("not get m_uart");
    return m_uart;
  }
}

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

void upper_packet_analys(void) {
  uint8_t  type     = rx_buf[0];
  uint16_t pack_len = rx_buf[4];
  uint8_t *recv_cmd;
  // bool     transmit_state = get_transmit_state();

  for (int i = 0; i < rx_len; i++)  //
    LOGI("%02x", rx_buf[i]);

  if (type == 0x70 || type == 0xd0) {
    recv_cmd = get_upper_cache();
    memcpy(recv_cmd, rx_buf, rx_len);
    set_upper_recv_len(rx_len);
    set_upper_process(true);
    LOGI("TYPE IS %02x", type);
  } else {
    LOGW("upper recv error %04x", type);
    return;
  }

  if (CRC16_Calculate(rx_buf, rx_len)) {
    LOGE("CRC error");
    return;
  }
}

void upper_receive_rx_data(void) {
  flag = true;
  enterQueueEvent(EVENT_UpperUart);
  rx_len = RECV_BUF_LEN - __HAL_DMA_GET_COUNTER(m_dma);
  LOGI("%d", rx_len);
  HAL_UARTEx_ReceiveToIdle_DMA(m_uart, rx_buf, sizeof(rx_buf));  // 串口�?�?+dma
  __HAL_DMA_DISABLE_IT(m_dma, DMA_IT_HT);                        // 关闭dma接收半满�?�?函数
}
