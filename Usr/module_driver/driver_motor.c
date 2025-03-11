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

Motor_t Servo;

FSM_STATE_t        moter_speed_set(uint16_t speed);
static FSM_STATE_t moter_set_move(ENUM_MOTOR_MOTION go_direction);
static uint16_t    get_servo_pos(void);
// void motor_move(void) {
//   Uart_Send_MovementRegister_ServoMotor(3, 1);  // 设置速度1，
// }

void servo_info_updata(uint8_t direct, uint8_t speed) {
  Servo.connect  = 1;
  Servo.pos      = get_Position_mm(GetMoterStateReg(), 4);
  Servo.speed    = speed;
  Servo.diretion = direct;
}

Motor_t get_servo_info(void) { return Servo; }

uint16_t get_Position_mm(uint8_t* buf, uint8_t len) {  // 计算获取当前距离
  uint16_t       Position_mm_buf;
  uint32_t       Position_um;
  uint32_t       time = HAL_GetTick();
  static uint8_t num;

  while (!Servo.uart_recv_state) {
    if (has_pass_time(time) > 50) {
      Uart_Send_MovementRegister_ServoMotor(0, 0);

    } else if (has_pass_time(time) > 500) {
      LOGW("time out");
      return 0xffff;
    }
  }
  Servo.uart_recv_state = false;
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

  if (Position_mm_buf > 5000) {
    return 0;
  }

  // LOGI("DIS %d", Position_mm_buf);
  return Position_mm_buf;
}

FSM_STATE_t moter_get_pos(uint16_t* pos) {
  static enum {
    START = 0,
    SEND_CMD_POS,
    RECV_SUCESS,
    WAIT_INFO,
  } s_State = {START};
  static uint32_t time;
  switch (s_State) {
    case START:
      s_State = SEND_CMD_POS;
      time    = HAL_GetTick();

    case SEND_CMD_POS:
      Uart_Send_MovementRegister_ServoMotor(0, 0);
      s_State = WAIT_INFO;
      break;
    case RECV_SUCESS:
      *pos    = get_Position_mm(GetMoterStateReg(), 4);
      s_State = START;
      return fsm_cpl;
      break;
      // case WAIT_INFO:
      //   if (has_pass_time(time) > 200) {
      //     LOGE("TIMER OUT");
      //     break;
      //   }

      //   break;
  }
  return fsm_onging;
}

uint16_t get_servo_pos(void) {
  uint16_t pos = 0;
  if (!moter_get_pos(&pos)) {
    LOGE("get pos error");
    return 0;
  }
  return pos;
}

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

      /*************Debug****************/
      LOGI("ack is %02x", ack[0]);
      LOGI("ack is %02x", ack[1]);
      /*********************************/
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

FSM_STATE_t servo_move(ENUM_MOTOR_MOTION direction) {
  uint32_t time = HAL_GetTick();
  uint8_t* ack  = NULL;
  uint16_t data = 0;
  while (!Servo.state) {
    if (has_pass_time(time) > 100) {
      LOGE("time out");
      return fsm_timeout;
    }

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