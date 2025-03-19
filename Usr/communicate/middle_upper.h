#pragma once

#include "middle_communicate_both.h"

typedef struct __attribute__((packed)) {
  uint8_t empty;
  uint8_t servo_conect;
  uint8_t servo_reset_state;
  uint8_t check_machine_connect;
  uint8_t check_machine_reset;
  uint8_t emergency_stop;
  uint8_t reader_connect;
  uint8_t radar_connect;
} Upper_Init_t;

void upper_communite_init(void);

void     notify_up_push(uint8_t cmd, uint8_t len, uint8_t* p_data, LOWER_NOTION notion);
Notion_t notify_up_pop(void);

void upp_send(void);

uint8_t* get_upper_cache(void);
void     set_upper_recv_len(uint8_t len);