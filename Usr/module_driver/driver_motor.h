#pragma once

#include "module_middle/middle_fsm.h"
#include "stdint.h"
#define ON_LEFT  -1
#define ON_RIGHT 1

typedef enum {
  KGoLeft  = 0x01,
  KGoRight = 0x02,

} ENUM_MOTOR_MOTION;

FSM_STATE_t motor_move(uint16_t speed, ENUM_MOTOR_MOTION go_direction);
FSM_STATE_t moter_speed_set(uint16_t speed);
FSM_STATE_t motor_stop(void);