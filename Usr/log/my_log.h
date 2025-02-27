#pragma once

int __io_putchar(int ch);

#define LOGI(fmt, ...) printf("[%08u] [INFO] <%.20s> : " fmt " \r\n", HAL_GetTick(), __func__, ##__VA_ARGS__);
#define LOGW(fmt, ...) printf("[%08u] [WARN] <%.20s> : " fmt " \r\n", HAL_GetTick(), __func__, ##__VA_ARGS__);
#define LOGE(fmt, ...) printf("[%08u] [EROR] <%.20s> : " fmt " \r\n", HAL_GetTick(), __func__, ##__VA_ARGS__);

#define vofa_LOGI(TYPE, fmt, ...) printf(" %s:" fmt "\n", TYPE, ##__VA_ARGS__)
