#pragma once
#include "stdbool.h"
#include "stdint.h"

void app_comunicate(void);
void app_communicate_init(void);

void set_upper_process(bool state);
void set_lower_process(bool state);

uint8_t get_action_mode(void);

void     book_ack_func(void);
uint32_t get_syn_time_data(void);