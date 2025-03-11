#pragma once

#include <stdint.h>

#define BOOK_DATABASE_CAPACITY 256

#pragma pack(1)

typedef struct {
  uint16_t x_cor;
  uint16_t y_cor;
  uint32_t UID_H;
  uint32_t UID_L;
  uint8_t  RSSI;
} BOOK_DATA_T;

typedef struct {
  uint8_t     index;
  BOOK_DATA_T data;

} BOOK_INFO_t;

#pragma pack()
