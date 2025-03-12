#include "driver_motor.h"

#include "ServoMotor.h"
#include "log/my_log.h"
#include "module_middle/middle_fsm.h"
#include "tick/tick.h"

#define SPEED 30

#define IS_SUCESS_RESET() \
  do {                    \
    s_State = START;      \
  } while (0)

// #define SET_SPEED_RESET() \
//   do {                    \
//     s_State = START;      \
//   } while (0)

// #define SET_MOVE_RESET() \
//   do {                   \
//     s_State = START;     \
//   } while (0)

Motor_t        Servo_info;
static Motor_t Servo_set;

void servo_info_updata(uint8_t direct, uint8_t speed) {
  Servo_info.connect  = 1;
  Servo_info.pos      = get_Position_mm(GetMoterStateReg(), 4);
  Servo_info.speed    = 1;
  Servo_info.diretion = direct;
}

void servo_info_set(uint8_t pos) { Servo_set.pos = pos; }

Motor_t* get_servo_set_info(void) { return &Servo_set; }
uint16_t get_servo_set_info_speed(void) { Servo_set.pos; }
Motor_t  get_servo_info(void) { return Servo_info; }

uint16_t get_Position_mm(uint8_t* buf, uint8_t len) {  // 计算获取当前距离
  uint16_t       Position_mm_buf;
  uint32_t       Position_um;
  uint32_t       time = HAL_GetTick();
  static uint8_t num;

  while (!Servo_info.uart_recv_state) {
    if (has_pass_time(time) > 100) {
      LOGW("time out");
      return 0xffff;
    }
    Uart_Send_MovementRegister_ServoMotor(0, 0);
  }
  Servo_info.uart_recv_state = false;
  // for (int i = 0; i < 4; i++) {
  //   LOGI("%02x", buf[i]);
  // }

  if (0x80 == (buf[2] & 0x80)) {  // 负方向，向右
    Position_um = 0xFFFFFFFF - (((uint32_t)buf[2] << 24) + ((uint32_t)buf[3] << 16) + ((uint32_t)buf[0] << 8) + ((uint32_t)buf[1] << 0));
    Position_um = Position_um + 1;
    LOGI("right");
    // Direction_of_position = MINUS_Right;
  } else if (0x00 == (buf[2] & 0x80)) {  // 正方向，向左
    Position_um = ((uint32_t)buf[2] << 24) + ((uint32_t)buf[3] << 16) + ((uint32_t)buf[0] << 8) + ((uint32_t)buf[1] << 0);
    LOGI("left");
    // Direction_of_position = PLUS_Left;
  }
  Position_mm_buf = (uint16_t)(Position_um / 1000);

  // if (Position_mm_buf > 5000) {
  //   return 0;
  // }

  // LOGI("DIS %d", Position_mm_buf);
  return Position_mm_buf;
}

FSM_STATE_t servo_speed_set(uint16_t speed) {
  uint32_t time = HAL_GetTick();
  uint8_t* ack  = NULL;
  uint16_t data = 0;
  while (!Servo_info.state) {
    if (has_pass_time(time) > 100) {
      LOGE("time out");
      return fsm_timeout;
    }
    Servo_info.uart_recv_state = false;

    Uart_Send_MovementRegister_ServoMotor(3, speed);

    ack = GetMoterAck();

    /*************Debug****************/
    LOGI("ack is %02x", ack[0]);
    LOGI("ack is %02x", ack[1]);
    /*********************************/
    if (NULL == ack) {
      LOGE("ack is null");
      return fsm_error;
    }

    if ((0x00 == ack[0] && speed == ack[1])) {
      LOGI("speed sucess");
      return fsm_cpl;
    }
  }
}

FSM_STATE_t servo_move(ENUM_MOTOR_MOTION direction) {
  uint32_t time = HAL_GetTick();
  uint8_t* ack  = NULL;
  uint16_t data = 0;
  while (!Servo_info.state) {
    if (has_pass_time(time) > 100) {
      LOGE("time out");
      return fsm_timeout;
    }
    Servo_info.uart_recv_state = false;

    if (ON_LEFT == GetMoterOnPos()) {
      if (KGoLeft == direction)
        Uart_Send_MovementRegister_ServoMotor(3, JOGmode_Data_CCW);
      else if (KGoRight == direction)
        Uart_Send_MovementRegister_ServoMotor(3, JOGmode_Data_CW);

    } else if (ON_RIGHT == GetMoterOnPos()) {
      if (KGoLeft == direction)
        Uart_Send_MovementRegister_ServoMotor(3, JOGmode_Data_CW);
      else if (KGoRight == direction)
        Uart_Send_MovementRegister_ServoMotor(3, JOGmode_Data_CCW);
    }

    ack = GetMoterAck();
    LOGI("ack is %02x", ack[0]);
    LOGI("ack is %02x", ack[1]);
    if (NULL == ack) {
      LOGE("ack is null");
      return fsm_error;
    }
    data = ack[0];
    data <<= 8;
    data |= ack[1];
    LOGI("data is %04x", data);
    //(((uint16_t)ack[4] << 8) | ack[5])
    if (JOGmode_Data_CCW == data ||  //
        JOGmode_Data_CW == data) {
      LOGI("Is success");
      return fsm_cpl;
    }
  }
}

// FSM_STATE_t check_book(void) {
//   static enum {
//     START,
//   } s_State = START;
// }