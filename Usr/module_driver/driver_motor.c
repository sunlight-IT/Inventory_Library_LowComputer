#include "driver_motor.h"

#include "ServoMotor.h"
#include "log/my_log.h"
#include "module_middle/middle_fsm.h"

static FSM_STATE_t moter_speed_set(void);
static FSM_STATE_t moter_set_move(void);
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

FSM_STATE_t motor_move(void) {
  static enum {
    START = 0,
    SET_SPEED,
    SET_MOVE,
  } s_State = {START};

  switch (s_State) {
    case START:
      s_State = SET_SPEED;
    case SET_SPEED:
      if (!moter_speed_set()) {
        // SET_SPEED_RESET();
        LOGE("wait speed set");
        s_State = SET_SPEED;
        break;
      }
      s_State = SET_MOVE;
    case SET_MOVE:
      if (!moter_set_move()) {
        LOGE("wait move set");
        s_State = SET_MOVE;
        break;
      }
      s_State = START;
      return fsm_cpl;
  }

  return fsm_onging;
}

FSM_STATE_t moter_speed_set(void) {
  static enum {
    START = 0,
    SEND_SPEED_CMD,
    IS_SUCESS,
  } s_State = {START};

  static uint8_t* ack;
  switch (s_State) {
    case START:
      s_State = SEND_SPEED_CMD;
    case SEND_SPEED_CMD:
      Uart_Send_MovementRegister_ServoMotor(3, 1);  // 设置速度
      s_State = IS_SUCESS;
      break;
    case IS_SUCESS:
      if (NULL == (ack = GetMoterAck())) {
        LOGE("ack is null");
        IS_SUCESS_RESET();
        return fsm_error;
      }

      if (0x00 == ack[0] && 0x01 == ack[1]) {
        LOGI("speed sucess");
        return fsm_cpl;
      }

      s_State = SEND_SPEED_CMD;
      break;
  }
  LOGI("onging");
  return fsm_onging;
}

FSM_STATE_t moter_set_move(void) {
  static enum {
    START = 0,
    SEND_MOVE_CMD,
    IS_SUCESS,
  } s_State = {START};

  static uint8_t* ack;
  switch (s_State) {
    case START:
      s_State = SEND_MOVE_CMD;
    case SEND_MOVE_CMD:
      if (ON_LEFT == GetMoterOnPos()) {
        Uart_Send_MovementRegister_ServoMotor(3, JOGmode_Data_CCW);
      } else if (ON_RIGHT == GetMoterOnPos()) {
        Uart_Send_MovementRegister_ServoMotor(3, JOGmode_Data_CW);
      }
      s_State = IS_SUCESS;
    case IS_SUCESS:
      if (NULL == (ack = GetMoterAck())) {
        LOGE("ack is null");
        IS_SUCESS_RESET();
        return fsm_error;
      }

      if (JOGmode_Data_CCW == (((uint16_t)ack[4] << 8) | ack[5]) ||  //
          JOGmode_Data_CW == (((uint16_t)ack[4] << 8) | ack[5])) {
        return fsm_cpl;
      }

      s_State = SEND_MOVE_CMD;
      break;
  }

  return fsm_onging;
}