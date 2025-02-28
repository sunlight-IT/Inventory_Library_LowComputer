#pragma once
#include "stdint.h"

#define EVENT_NUM 16

typedef void (*event_process_func)(void);

typedef enum {
  kPackIdle,
  kWireLess,
  kMoterUart,
  kCmd,
  // 预留事件处理函数
} ENUM_DATA_PACK_EVENT;

typedef struct {
  ENUM_DATA_PACK_EVENT type;
  event_process_func   func;
  // 预留事件处理函数
} Event_t;

// typedef struct {
//   uint8_t  num;
//   uint32_t time;
//   uint8_t  RSSI;
//   uint8_t  UID[8];
// } BOOK_DATA_t;

void reg_event(Event_t *h_event);
void event_datapack_process(void);
void event_data_book(const uint8_t *data, uint8_t len);