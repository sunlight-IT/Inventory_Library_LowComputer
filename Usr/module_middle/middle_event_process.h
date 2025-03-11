#pragma once
#include "stdint.h"

#define EVENT_QUEUE_SIZE 16

typedef void (*event_callback)(void);

typedef enum {
  EVENT_Wirless,
  EVENT_UpperUart,
  EVENT_MotorUart,
  // kTrace,
  // kShow,
  // 事件处理
  EVENT_MAX,
} EVENT_TYPE;

typedef struct {
  EVENT_TYPE     type;
  event_callback callback;

  // 事件处理函数
} EVENT_t;

void registCallback(EVENT_TYPE type, event_callback cb);
void enterQueueEvent(EVENT_TYPE type);
void process_event(void);

// typedef struct {
//   uint8_t  num;
//   uint32_t time;
//   uint8_t  RSSI;
//   uint8_t  UID[8];
// } BOOK_DATA_t;

// void attach_event(EVENT_t *h_event);
// void event_datapack_process(void);
// void event_data_book(const uint8_t *data, uint8_t len);