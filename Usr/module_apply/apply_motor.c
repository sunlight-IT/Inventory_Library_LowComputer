#include "apply_motor.h"

#include "communicate/middle_communicate_both.h"
#include "log/my_log.h"
#include "module_driver/driver_motor.h"
#include "module_driver/driver_wireless.h"
#include "module_middle/middle_event_process.h"
#include "module_middle/middle_fsm.h"
#include "tick/tick.h"

static enum {
  START,
  SETUP_RESET_POS,
  MOVING,
  STOP,
} s_State_Servo = SETUP_RESET_POS;

static Moter_Config_t* p_servo_config;

static uint8_t ack_reset;

static FSM_STATE_t app_move(void);
static FSM_STATE_t app_reset(void);

void app_servo_action_init(void) { p_servo_config = get_servo_config(); }

void app_action(void) {
  switch (s_State_Servo) {
    case SETUP_RESET_POS:
      if (p_servo_config->motion_mode != kResetPos) {
        break;
      }

      if (!app_reset()) {
        break;
      }

      ack_reset = 0x02;
      notify_up_push(kResetPos, 5, &ack_reset, kSend);
      p_servo_config->motion_mode = 0;
      s_State_Servo               = STOP;
      break;
    case STOP:
      if (p_servo_config->motion_mode == kMoveAction) {
        s_State_Servo = MOVING;
      } else if (p_servo_config->motion_mode == kResetMode) {
        s_State_Servo = SETUP_RESET_POS;
      }
      break;
    case MOVING:

      if (p_servo_config->motion_mode == kMoveAction) {
        if (!app_move()) {
          break;
        }
        p_servo_config->motion_mode = 0;
        s_State_Servo               = STOP;
      }
      break;
  }
}

FSM_STATE_t app_move(void) {
  static enum {
    START,
    MOVE,
    ARRIVE_WAIT,
    STOP,
  } s_State = START;
  static int16_t pos_cur;
  static int16_t pos_tar;
  uint8_t        direction_moving;
  switch (s_State) {
    case START:

      pos_tar = p_servo_config->pos;  // get_servo_config_pos();
      s_State = MOVE;
    case MOVE:
      pos_cur = get_Position_mm(GetMoterStateReg(), 4);
      // if (ON_LEFT == p_servo_config->location) {
      //   direction_moving =
      // }
      if (pos_cur > pos_tar + 10) {
        direction_moving = KGoLeft;
      } else if (pos_cur < pos_tar - 10) {
        direction_moving = KGoRight;
      } else if (pos_cur >= pos_tar - 10 && pos_cur <= pos_tar + 10) {
        s_State = STOP;
        break;
      }

      if (servo_move(direction_moving)) {
        servo_info_updata(direction_moving, p_servo_config->speed);
        s_State = ARRIVE_WAIT;
      }
      break;
    case ARRIVE_WAIT:
      pos_cur = get_Position_mm(GetMoterStateReg(), 4);
      LOGI("pos_tar1 is :%04x", pos_tar);
      LOGI("cur pos is %04x", pos_cur);
      if (pos_cur >= pos_tar - 10 && pos_cur <= pos_tar + 10 && pos_cur != 0xff) {
        s_State = STOP;
      }
      break;
    case STOP:
      if (servo_speed_set(0)) {
        s_State = START;
        return fsm_cpl;
      }

      break;
  }

  return fsm_onging;
}

FSM_STATE_t app_reset(void) {
  static enum {
    START,
    MOVE,
    ARRIVE_WAIT,
    CLEAR_PLUSE,
    STOP,
  } s_State = START;
  uint8_t                direction_moving;
  static Moter_Config_t* servo_config;
  switch (s_State) {
    case START:
      servo_config = get_servo_config();
      s_State      = MOVE;
      if (ON_LEFT == servo_config->location) {
        direction_moving = KGoLeft;
      } else if (ON_RIGHT == servo_config->location) {
        direction_moving = KGoRight;
      }
    case MOVE:
      if (p_servo_config->reset_mode == 0x01) {
        s_State = ARRIVE_WAIT;
      } else if (p_servo_config->reset_mode == 0x02) {  // 先设0x02为自动复位

        if (servo_move(direction_moving)) {
          LOGI("wait reset pos");
          s_State = ARRIVE_WAIT;
        }
      }

      break;

    case ARRIVE_WAIT:
      if (LimitSensorON_ServoMotor == HAL_GPIO_ReadPin(LimitSensor_ServoMotor_GPIO_Port, LimitSensor_ServoMotor_Pin)) {
        s_State = CLEAR_PLUSE;
        LOGI("ON");
        break;
      }
      // } else if (LimitSensorOFF_ServoMotor == HAL_GPIO_ReadPin(LimitSensor_ServoMotor_GPIO_Port, LimitSensor_ServoMotor_Pin)) {
      //   LOGI("OFF");
      // }
      //  LOGI("ARRIVE WAIT");
      s_State = ARRIVE_WAIT;
      break;

    case CLEAR_PLUSE:
      if (!Clear_Pluse()) {
        s_State = STOP;
        break;
      }
      break;
    case STOP:

      if (servo_speed_set(0)) {
        LOGI("RESET POS SUCESS");

        servo_config_updata(kResetState, 0x01);
        s_State = START;
        return fsm_cpl;
      }
      s_State = STOP;
      break;
  }
  return fsm_onging;
}
