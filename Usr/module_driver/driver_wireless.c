#include "driver_wireless.h"

#include "../log/my_log.h"
#include "component/cmd_process/cmd_process.h"
#include "module_middle/middle_event_process.h"
#include "stdbool.h"
static UART_HandleTypeDef *m_uart;
static DMA_HandleTypeDef  *m_dma;

static uint8_t rx_buf[WIRELESS_MAX_BUF_LEN];
static uint8_t rx_len;

static uint8_t rx_cache;
static bool    flag = false;

static uint8_t cmd = 0xff;

static uint8_t book_buf[WIRELESS_MAX_BUF_LEN];

static void wireless_packet_analys(void);

void wireless_reg_handle(UART_HandleTypeDef *h_uart, DMA_HandleTypeDef *h_dma) {
  m_uart = h_uart;
  m_dma  = h_dma;
}

void wireless_init(void) {
  registCallback(EVENT_Wirless, wireless_packet_analys);         // 注册事件表
  HAL_UARTEx_ReceiveToIdle_DMA(m_uart, rx_buf, sizeof(rx_buf));  // 串口�?�?+dma
  __HAL_DMA_DISABLE_IT(m_dma, DMA_IT_HT);                        // 关闭dma接收半满�?�?函数
}

UART_HandleTypeDef *GetWireLessHandle(void) {
  if (m_uart == NULL) LOGW("not get m_uart");
  return m_uart;
}

void wireless_send(uint8_t *data, uint8_t len) { HAL_UART_Transmit(m_uart, data, len, 100); }

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

void wireless_packet_analys(void) {
  uint8_t  type     = rx_buf[0];
  uint16_t pack_len = rx_buf[4];
  uint8_t *recv_cmd;
  bool     transmit_state = get_transmit_state();
  LOGI("WIRLESS");
  if (type == 0x70) {
    if (CRC16_Calculate(&rx_buf, 6 + pack_len)) {
      LOGE("CRC error");
      return;
    }
    // lower computer recv
    if (transmit_state) {
      LOGE("transmit has occupied");
      return;
    }

    transmit_using(true);
    recv_cmd = get_cmd_cache();
    memcpy(recv_cmd, rx_buf, rx_len);
    set_cmd_recv_len(rx_len);
  }
  // pack_len |= rx_buf[0];
  // pack_len <<= 8;
  // pack_len |= rx_buf[1];
  // LOGI("%04x", pack_len);
  // if (rx_len == 9) {
  //   cmd    = rx_buf[5];
  //   rx_len = 0;
  // } else if (pack_len > 5) {
  //   if (CRC16_Calculate(&rx_buf, pack_len)) {
  //     LOGE("CRC error");
  //     return;
  //   }

  // event_data_book(&(rx_buf[2]), pack_len);
  // }
}

void wireless_receive(void) {
  if (flag) {
    for (int i = 0; i < rx_len; i++) LOGI("%02x", rx_buf[i]);
    // LOGI("%02x", wireless_packet_analys())
    flag = false;
  }
}
void ClearCmd(void) { cmd = 10; }

uint8_t GetCmdType(void) {
  // if (flag) {
  // LOGI("Cmd is : %02x", cmd);
  // flag = false;
  return cmd;
  //}
  // return 10;
}

void receive_rx_data(void) {
  flag = true;
  enterQueueEvent(EVENT_Wirless);
  rx_len = WIRELESS_MAX_BUF_LEN - __HAL_DMA_GET_COUNTER(m_dma);
  LOGI("%d", rx_len);
  HAL_UARTEx_ReceiveToIdle_DMA(m_uart, rx_buf, sizeof(rx_buf));  // 串口�?�?+dma
  __HAL_DMA_DISABLE_IT(m_dma, DMA_IT_HT);                        // 关闭dma接收半满�?�?函数
}
