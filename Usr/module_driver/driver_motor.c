#include "driver_motor.h"

#include "ServoMotor.h"
#include "log/my_log.h"
#include "module_middle/middle_fsm.h"

FSM_STATE_t        moter_speed_set(uint16_t speed);
static FSM_STATE_t moter_set_move(ENUM_MOTOR_MOTION go_direction);
// void motor_move(void) {
//   Uart_Send_MovementRegister_ServoMotor(3, 1);  // 设置速度1，
// }

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

FSM_STATE_t motor_move(uint16_t speed, ENUM_MOTOR_MOTION go_direction) {
  static enum {
    START = 0,
    SET_SPEED,
    SET_MOVE,
  } s_State = {START};

  switch (s_State) {
    case START:
      s_State = SET_SPEED;
    case SET_SPEED:
      if (!moter_speed_set(speed)) {
        // SET_SPEED_RESET();
        LOGE("wait speed set");
        s_State = SET_SPEED;
        break;
      }
      s_State = SET_MOVE;
    case SET_MOVE:
      if (!moter_set_move(go_direction)) {
        LOGE("wait move set");
        s_State = SET_MOVE;
        break;
      }
      s_State = START;
      return fsm_cpl;
  }

  return fsm_onging;
}

FSM_STATE_t motor_stop(void) { return moter_speed_set(0); }

FSM_STATE_t moter_speed_set(uint16_t speed) {
  static enum {
    START = 0,
    SEND_SPEED_CMD,
    SEND_WAITE,
    IS_SUCESS,
  } s_State = {START};

  static uint8_t* ack;
  switch (s_State) {
    case START:
      s_State = SEND_SPEED_CMD;
    case SEND_SPEED_CMD:
      Uart_Send_MovementRegister_ServoMotor(3, speed);  // 设置速度
      s_State = SEND_WAITE;
    case SEND_WAITE:
      s_State = IS_SUCESS;
      break;
    case IS_SUCESS:
      ack = GetMoterAck();
      LOGI("ack is %02x", ack[0]);
      LOGI("ack is %02x", ack[1]);
      if (NULL == ack) {
        LOGE("ack is null");
        IS_SUCESS_RESET();
        return fsm_error;
      }

      if ((0x00 == ack[0] && speed == ack[1])) {
        LOGI("speed sucess");
        return fsm_cpl;
      }

      s_State = SEND_SPEED_CMD;
      break;
  }
  LOGI("onging");
  return fsm_onging;
}

FSM_STATE_t moter_set_move(ENUM_MOTOR_MOTION direction) {
  static enum {
    START = 0,
    SEND_MOVE_CMD,
    SEND_WAITE,
    IS_SUCESS,
  } s_State = {START};

  static uint8_t* ack;
  uint16_t        data;
  switch (s_State) {
    case START:
      s_State = SEND_MOVE_CMD;
      data    = 0;
      LOGI("Send start")
    case SEND_MOVE_CMD:

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

      s_State = SEND_WAITE;
    case SEND_WAITE:
      s_State = IS_SUCESS;
      // LOGI("send waite");
      break;
    case IS_SUCESS:
      ack = GetMoterAck();
      LOGI("ack is %02x", ack[0]);
      LOGI("ack is %02x", ack[1]);
      if (NULL == ack) {
        LOGE("ack is null");
        IS_SUCESS_RESET();
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

      s_State = START;
      break;
  }
  LOGI(" mov onging");
  return fsm_onging;
}