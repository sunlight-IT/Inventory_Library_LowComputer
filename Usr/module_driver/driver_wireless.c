#include "driver_wireless.h"

#include "../log/my_log.h"
#include "module_middle/middle_event_process.h"
#include "stdbool.h"
static UART_HandleTypeDef *m_uart;
static DMA_HandleTypeDef  *m_dma;

static uint8_t rx_buf[WIRELESS_MAX_BUF_LEN];
static uint8_t rx_len;

static uint8_t rx_cache;
static bool    flag = false;

static uint8_t cmd = 0xff;
static Event_t wireless_event;

static uint8_t book_buf[WIRELESS_MAX_BUF_LEN];

static uint8_t book_database[][14];

static void wireless_packet_analys(void);

void wireless_reg_handle(UART_HandleTypeDef *h_uart, DMA_HandleTypeDef *h_dma) {
  m_uart = h_uart;
  m_dma  = h_dma;
}
void wireless_event_init(void) {
  wireless_event.type = kPackIdle;
  wireless_event.func = wireless_packet_analys;
}

void wireless_init(void) {
  wireless_event_init();  // 无线事件初始化
  reg_event(&wireless_event);

  HAL_UARTEx_ReceiveToIdle_DMA(m_uart, rx_buf, sizeof(rx_buf));  // 串口中断+dma
  __HAL_DMA_DISABLE_IT(m_dma, DMA_IT_HT);                        // 关闭dma接收半满中断函数
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
        crc16 = (crc16 >> 1) ^ 0x8408;
      } else {
        crc16 = (crc16 >> 1);
      }
    }
  }
  LOGI("CRC is :%04x", crc16);
  return crc16;
}

void wireless_packet_analys(void) {
  uint16_t pack_len = rx_buf[0];

  pack_len <<= 8;
  pack_len = rx_buf[1];
  LOGI("%04x", pack_len);
  if (rx_len == 9) {
    cmd    = rx_buf[5];
    rx_len = 0;
  } else if (pack_len > 5) {
    if (CRC16_Calculate(&rx_buf, pack_len)) {
      LOGE("CRC error");
      return;
    }
    //  for (int i = 0; i < pack_len - 4; i++) {
    //    LOGI("%02x", book_buf[i]);
    //  }

    event_data_book(&(rx_buf[2]), 14);
  }
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
  flag                = true;
  wireless_event.type = kWireLess;
  rx_len              = WIRELESS_MAX_BUF_LEN - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
  LOGI("%d", rx_len);
  // wireless_packet_analys();
  HAL_UARTEx_ReceiveToIdle_DMA(m_uart, rx_buf, sizeof(rx_buf));  // 串口中断+dma
  __HAL_DMA_DISABLE_IT(m_dma, DMA_IT_HT);                        // 关闭dma接收半满中断函数
}
