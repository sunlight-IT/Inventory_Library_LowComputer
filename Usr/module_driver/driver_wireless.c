#include "driver_wireless.h"
#include "../log/my_log.h"
#include "stdbool.h"
#include "module_middle/middle_event_process.h"
static UART_HandleTypeDef *m_uart;
static DMA_HandleTypeDef *m_dma;

static uint8_t rx_buf[WIRELESS_MAX_BUF_LEN];
static uint8_t rx_len;

// static uint8_t wireless_buf[]

static uint8_t rx_cache;
static bool flag = false;

static uint8_t cmd;
static Event_t wireless_event;

static void wireless_packet_analys(void);

void wireless_reg_handle(UART_HandleTypeDef *h_uart, DMA_HandleTypeDef *h_dma)
{
    m_uart = h_uart;
    m_dma = h_dma;
}
void wireless_event_init(void)
{
    wireless_event.type = kPackIdle;
    wireless_event.func = wireless_packet_analys;
}

void wireless_init(void)
{
    wireless_event_init(); // 无线事件初始化
    reg_event(&wireless_event);

    HAL_UARTEx_ReceiveToIdle_DMA(m_uart, rx_buf, sizeof(rx_buf)); // 串口中断+dma
    __HAL_DMA_DISABLE_IT(m_dma, DMA_IT_HT);                       // 关闭dma接收半满中断函数
}

UART_HandleTypeDef *GetWireLessHandle(void)
{
    if (m_uart == NULL)
        LOGW("not get m_uart");
    return m_uart;
}

void wireless_send(uint8_t *data, uint8_t len)
{
    HAL_UART_Transmit(m_uart, data, len, 100);
}

void wireless_packet_analys(void)
{
    // RecvPackHead_t *head;
    // head
    if (rx_len == 5)
    {
        cmd = rx_buf[3];
    }
    LOGI("EVENT FUNC success");
}

void wireless_receive(void)
{
    if (flag)
    {
        for (int i = 0; i < rx_len; i++)
            LOGI("%02x", rx_buf[i]);
        // LOGI("%02x", wireless_packet_analys())
        flag = false;
    }
}

uint8_t GetCmdType(void)
{

    if (flag)
    {
        LOGI("Cmd is : %02x", cmd);
        flag = false;
        return cmd;
    }
    return 10;
}

void receive_rx_data(void)
{
    flag = true;
    wireless_event.type = kWireLess;
    LOGI("TEST");
    rx_len = WIRELESS_MAX_BUF_LEN - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
    HAL_UARTEx_ReceiveToIdle_DMA(m_uart, rx_buf, sizeof(rx_buf)); // 串口中断+dma
    __HAL_DMA_DISABLE_IT(m_dma, DMA_IT_HT);                       // 关闭dma接收半满中断函数
}
