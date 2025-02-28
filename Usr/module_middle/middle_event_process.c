#include "middle_event_process.h"

#include <stdint.h>
#include <string.h>

#include "log/my_log.h"
// #include "module_driver/driver_wireless.h"

// static BOOK_DATA_t book_database[128];
static uint8_t book_database[][20];

static Event_t *event[EVENT_NUM];
static uint8_t  index_event;

void reg_event(Event_t *h_event) {
  // event
  if (index_event >= EVENT_NUM) {
    LOGI("event num overflow");
    return;
  }

  event[index_event] = h_event;
  index_event++;
}

void event_datapack_process(void) {
  for (int i = 0; i < index_event; i++) {
    if (kPackIdle != event[i]->type) {
      if (NULL == event[i]->func) {
        LOGE("event func is empty: %d", i);
        return;
      }
      // 相对应的函数

      event[i]->func();
      event[i]->type = kPackIdle;
    }
  }
}

void event_cmd_process(void) {}

void event_data_book(const uint8_t *data, uint8_t len) {
  uint8_t book_num = len / 14;
  for (int i = 0; i < book_num; i++) {
    memcpy(book_database[i], &(data[i * 14]), 14);
  }
  LOGI("SUCESS");

  for (int j = 0; j < 14; j++) {
    LOGI("%02x", book_database[0][j]);
  }
}