#pragma once

#include <stdint.h>

#include "module_middle/middle_fsm.h"
#include "usart.h"
#define ON_LEFT  -1
#define ON_RIGHT 1

// typedef enum {
//   kMotion = 0xa1,
//   kRadarError,        // 雷达错误
//   kCollideError,      // 碰撞错误
//   kLostContactError,  // 失联
//   kLastMotionCpl,
//   knonMotion = 0xaa,
// } ENUM_ACTION_STATE;

// 1字节对齐
typedef struct __attribute__((packed)) {
  // void*    peripheral;  // 伺服电机使用外设
  uint16_t state;
  uint16_t connect;
  uint16_t pos;
  uint16_t speed;
  uint8_t  diretion;

  bool uart_recv_state;
} Motor_t;

typedef enum {
  KGoLeft  = 0xCC,
  KGoRight = 0x0C,
} ENUM_MOTOR_MOTION;

extern Motor_t Servo;

FSM_STATE_t motor_move(uint16_t speed, ENUM_MOTOR_MOTION go_direction);
FSM_STATE_t moter_speed_set(uint16_t speed);
FSM_STATE_t motor_stop(void);

Motor_t get_servo_info(void);

void servo_info_updata(uint8_t direct, uint8_t speed);

uint16_t get_Position_mm(uint8_t* buf, uint8_t len);