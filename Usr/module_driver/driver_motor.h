#pragma once

#include <stdint.h>

#include "module_middle/middle_fsm.h"
#include "usart.h"
#define ON_LEFT  -1
#define ON_RIGHT 1

#define UNIT_SPEED_CLOCK_RATIO(v) (1000 / v)

// 速度设置的分辨率为，1mm/s
// 设置为30  30mm/s
// 时间用   10mm 为分辨率  0.001 mm/ms   =    0.01 mm/ 10ms
// 单位坐标为 1mm  ，时间单位怎么定，1个时间为1mm,假设速度为30mm/s, 1mm时间为 30ms

// 所以不同速度需要设置不同的时间单位
// 时间单位和速度有关
// 设置的速度为 v mm/s   ,1mm 时间为 1/v s =  1000/v ms

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
  // uint8_t  reset_state;
  bool uart_recv_state;
} Motor_t;

typedef struct __attribute__((packed)) {
  uint8_t connect;
  uint8_t reset_state;
  uint8_t stop_switch;
  uint8_t reader_connect;
  uint8_t radar_connect;
} Check_Device_Info_t;

typedef enum {
  kConnectCheck,
  kRest,
  kStopSwitch,
  kReader,
  kRadar,
} ENUM_CONFIG_CHECK_DIVICE;

typedef struct __attribute__((packed)) {
  uint16_t reset_state;
  uint16_t connect;      // 伺服电机连接
  uint16_t pos;          // 配置位置
  uint16_t speed;        // 配置速度
  uint8_t  diretion;     // 配置方向
  uint8_t  location;     // 当前位置
  uint8_t  reset_mode;   // 配置复位模式
  uint8_t  motion_mode;  // 运动模式
} Moter_Config_t;

typedef enum {
  kResetState,
  kConnect,
  kPos,
  kSpeed,
  kDirection,
  kLocation,
  kResetMode,

  kActionMode,
} ENUM_CONFIG_SERVO;

typedef enum {
  kConnectInfo,
  kPosInfo,
  kSpeedInfo,
  kDirectionInfo,
  kResetInfo,
} ENUM_SERVO_INFO;

typedef enum {
  KGoLeft  = 0xCC,
  KGoRight = 0x0C,
} ENUM_MOTOR_MOTION;

// typedef enum {
//   KGoLeft  = 0xCC,
//   KGoRight = 0x0C,
// } ENUM_MOTOR_MOTION;

// typedef enum {
//   RESET,
//   MOVE,
// } SERVO_ACTION_MODE;

extern Motor_t Servo_info;

void servo_config_init(void);

void servo_info_updata(ENUM_SERVO_INFO info_type, uint16_t info);
void servo_config_updata(ENUM_CONFIG_SERVO config_type, uint16_t config_info);

Moter_Config_t* get_servo_config(void);
uint16_t        get_servo_config_pos(void);
uint16_t        get_servo_config_speed(void);
uint8_t         get_servo_config_reset_state(void);
FSM_STATE_t     servo_speed_set(uint16_t speed);

FSM_STATE_t Clear_Pluse(void);

FSM_STATE_t servo_move(ENUM_MOTOR_MOTION direction);

Motor_t* get_servo_info(void);

uint16_t get_Position_mm(uint8_t* buf, uint8_t len);

FSM_STATE_t clear_pluse_end(void);

FSM_STATE_t clear_pluse_start(void);