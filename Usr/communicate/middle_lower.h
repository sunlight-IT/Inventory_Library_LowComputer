#pragma once

#include "middle_communicate_both.h"

void lower_communite_init(void);

void     notify_low_send_push(uint8_t cmd, uint8_t len, uint8_t* p_data, LOWER_NOTION notion);
Notion_t notify_low_send_pop(void);

void     notify_low_recv_push(uint8_t cmd, uint8_t len, uint8_t* p_data, LOWER_NOTION notion);
Notion_t notify_low_recv_pop(void);

void low_send(void);

uint8_t* get_lower_cache(void);
uint8_t* get_lower_send_cache(void);

void set_lower_send_len(uint8_t len);
void set_lower_recv_len(uint8_t len);

void set_lower_book_ack(uint32_t book_index);