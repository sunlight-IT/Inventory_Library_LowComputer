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

extern Motor_t Servo_info;

void     servo_info_set(uint8_t pos);
Motor_t* get_servo_set_info(void);
uint16_t get_servo_set_info_speed(void);

FSM_STATE_t servo_speed_set(uint16_t speed);

FSM_STATE_t servo_move(ENUM_MOTOR_MOTION direction);

Motor_t get_servo_info(void);

void servo_info_updata(uint8_t direct, uint8_t speed);

uint16_t get_Position_mm(uint8_t* buf, uint8_t len);