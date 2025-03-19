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

Motor_t               Servo_info;
Check_Device_Info_t   check_info;
static Moter_Config_t Servo_Config;

void servo_config_init(void) {
  Servo_Config.location    = ON_LEFT;  // 伺服电机位置
  Servo_Config.speed       = 30;
  Servo_Config.reset_state = 0;  // 未复位
  Servo_Config.connect     = 1;  // 已经连接
}

void servo_info_updata(ENUM_SERVO_INFO info_type, uint16_t info) {
  switch (info_type) {
    case kConnectInfo:
      Servo_info.connect = info;
      break;
    case kPosInfo:
      Servo_info.pos = info;
      break;
    case kSpeedInfo:
      Servo_info.speed = info;
      break;
    case kDirectionInfo:
      Servo_info.diretion = (uint8_t)info;
      break;
      // case kResetInfo:
      //   Servo_info.reset_state = (uint8_t)info;
      //   break;
  }
}

void check_info_set(ENUM_CONFIG_CHECK_DIVICE config_type, uint8_t config_info) {
  switch (config_type) {
    case kConnectCheck:
      check_info.connect = config_info;
      break;
    case kRest:
      if (config_info >= 1000) {
        config_info = 1000;
      } else if (config_info <= 0) {
        config_info = 0;
      }

      check_info.reset_state = config_info;
      break;
    case kStopSwitch:
      if (config_info >= 100) {
        config_info = 100;
      } else if (config_info <= 0) {
        config_info = 0;
      }
      check_info.stop_switch = config_info;
      break;
    case kReader:
      check_info.reader_connect = (uint8_t)config_info;
      break;
    case kRadar:
      check_info.radar_connect = (uint8_t)config_info;
      break;
  }
}

void servo_config_updata(ENUM_CONFIG_SERVO config_type, uint16_t config_info) {
  switch (config_type) {
    case kResetState:
      Servo_Config.reset_state = config_info;
      break;
    case kConnect:
      Servo_Config.connect = config_info;
      break;
    case kPos:

      if (config_info >= 1000) {
        config_info = 1000;
      } else if (config_info <= 0) {
        config_info = 0;
      }

      Servo_Config.pos = config_info;
      LOGI("SERVO POS %04X", Servo_Config.pos);
      LOGI("SERV1 POS %04X", config_info);

      break;
    case kSpeed:
      if (config_info >= 100) {
        config_info = 100;
      } else if (config_info <= 0) {
        config_info = 0;
      }
      Servo_Config.speed = config_info;
      break;
    case kDirection:
      Servo_Config.diretion = (uint8_t)config_info;
      break;
    case kResetMode:
      Servo_Config.reset_mode = (uint8_t)config_info;
      break;
    case kActionMode:
      Servo_Config.motion_mode = (uint8_t)config_info;
      break;
  }
}

Moter_Config_t* get_servo_config(void) { return &Servo_Config; }
uint16_t        get_servo_config_speed(void) { return Servo_Config.speed; }
uint8_t         get_servo_config_reset_state(void) { return Servo_Config.reset_state; }

uint16_t get_servo_config_pos(void) {
  LOGI("TAR POS IS : %04x", Servo_Config.pos);
  return Servo_Config.pos;
}

Motor_t* get_servo_info(void) { return &Servo_info; }

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
  while (!Servo_info.uart_recv_state) {
    if (has_pass_time(time) > 500) {
      LOGE("time out");
      return fsm_timeout;
    }
    Uart_Send_MovementRegister_ServoMotor(3, speed);
  }
  Servo_info.uart_recv_state = false;
  ack                        = GetMoterAck();

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

  return fsm_onging;
}

FSM_STATE_t servo_move(ENUM_MOTOR_MOTION direction) {
  uint32_t        time         = HAL_GetTick();
  uint8_t*        ack          = NULL;
  uint16_t        data         = 0;
  Moter_Config_t* servo_config = get_servo_config();
  while (!Servo_info.uart_recv_state) {
    if (has_pass_time(time) > 100) {
      LOGE("time out");
      return fsm_timeout;
    }

    if (ON_LEFT == servo_config->location) {
      if (KGoLeft == direction)
        Uart_Send_MovementRegister_ServoMotor(3, JOGmode_Data_CCW);
      else if (KGoRight == direction)
        Uart_Send_MovementRegister_ServoMotor(3, JOGmode_Data_CW);

    } else if (ON_RIGHT == servo_config->location) {
      if (KGoLeft == direction)
        Uart_Send_MovementRegister_ServoMotor(3, JOGmode_Data_CW);
      else if (KGoRight == direction)
        Uart_Send_MovementRegister_ServoMotor(3, JOGmode_Data_CCW);
    }
  }

  Servo_info.uart_recv_state = false;
  ack                        = GetMoterAck();
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

  return fsm_onging;
}

FSM_STATE_t clear_pluse_start(void) {
  uint32_t time = HAL_GetTick();
  uint8_t* ack  = NULL;
  uint16_t data = 0;

  while (!Servo_info.uart_recv_state) {
    if (has_pass_time(time) > 100) {
      LOGE("time out");
      return fsm_timeout;
    }
    Uart_Send_MovementRegister_ServoMotor(4, DI2_PulseClear_Start_Data_P2_11);
  }
  Servo_info.uart_recv_state = false;

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
  if (DI2_PulseClear_Start_Data_P2_11 == data) {
    LOGI("clear start plus is success");
    return fsm_cpl;
  }

  return fsm_onging;
}

FSM_STATE_t clear_pluse_end(void) {
  uint32_t time = HAL_GetTick();
  uint8_t* ack  = NULL;
  uint16_t data = 0;

  while (!Servo_info.uart_recv_state) {
    if (has_pass_time(time) > 100) {
      LOGE("time out");
      return fsm_timeout;
    }
    Uart_Send_MovementRegister_ServoMotor(4, DI2_PulseClear_End_Data_P2_11);
  }

  Servo_info.uart_recv_state = false;

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
  if (DI2_PulseClear_End_Data_P2_11 == data) {
    LOGI("clear plus stop is success");
    return fsm_cpl;
  }

  return fsm_onging;
}

FSM_STATE_t Clear_Pluse(void) {
  static enum {
    START,
    CLEAR_START,
    CLEAR_END,
  } s_State = START;
  switch (s_State) {
    case START:
      s_State = CLEAR_START;

    case CLEAR_START:
      if (!clear_pluse_start()) {
        break;
      }
      s_State = CLEAR_END;
      break;
    case CLEAR_END:
      if (!clear_pluse_end()) {
        break;
      }
      s_State = START;
      return fsm_cpl;
      break;
  }

  return fsm_onging;
}