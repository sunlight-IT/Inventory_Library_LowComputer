#include "apply_motor.h"

#include "log/my_log.h"
#include "module_driver/driver_motor.h"
#include "module_driver/driver_wireless.h"
#include "module_middle/middle_event_process.h"
#include "module_middle/middle_fsm.h"
#include "tick/tick.h"

static bool emergrency_error;

static uint8_t  pandian_flag;
static uint32_t pandian_time;

static uint8_t cmd;

void cmd_process(void);

FSM_STATE_t check_book(ENUM_MOTOR_MOTION go_direction) {
  static enum {
    START = 0,
    MOTOR_MOVING,
    MOTOR_WAITE,
    MOTOR_STOP,
  } s_State = START;

  static uint32_t waiting_time;
  static bool     direction;  // false 方向为正，true方向为反

  if (emergrency_error) {
    s_State   = START;
    direction = false;
  }

  switch (s_State) {
    case START:
      s_State = MOTOR_MOVING;
    case MOTOR_MOVING:
      if (direction == false) {
        if (!motor_move(30, go_direction)) {
          break;
        }
      }
      waiting_time = HAL_GetTick();
      direction    = true;
      go_direction = -go_direction;
      s_State      = MOTOR_WAITE;
    case MOTOR_WAITE:
      if (has_pass_time(waiting_time) < 12000) {
        break;
      }
      s_State = MOTOR_STOP;
    case MOTOR_STOP:
      // if (!motor_stop()) {
      //   break;
      // }

      if (direction) {
        s_State = MOTOR_MOVING;
      } else {
        s_State = START;
        return fsm_cpl;
      }
  }
  return fsm_onging;
}

// void cmd_process(void) {
//   cmd = GetCmdType();
//   switch (cmd) {
//     case kWirelessIdle:
//       break;
//     case kWirelessStart:
//       if (!check_book(KGoRight))
//         //   if (motor_move(30, KGoRight)) {
//         //     ClearCmd();
//         //     pandian_flag = 1;
//         //     pandian_time = HAL_GetTick();
//         //   }
//         break;
//     case kWirelessStop:
//       if (motor_stop()) {
//         ClearCmd();
//         emergrency_error = true;
//         pandian_flag     = 0xff;
//       }
//       break;
//     case kBook:
//       break;
//   }
//   // check process
//   switch (pandian_flag) {
//     case 1:
//       if (has_pass_time(pandian_time) > 12000) {
//         // moter_speed_set(0);

//         pandian_flag = 2;
//         LOGI("check finish %d", pandian_flag);
//       }

//       break;
//     case 2:
//       if (motor_stop()) {
//         pandian_flag = 3;
//         // LOGI("Back");
//         LOGI("Back %d ", pandian_flag);
//       }

//       break;
//     case 3:
//       if (motor_move(30, KGoLeft)) {
//         pandian_time = HAL_GetTick();
//         pandian_flag = 4;

//         LOGI("Back move %d", pandian_flag);
//       }

//       break;
//     case 4:
//       if (has_pass_time(pandian_time) > 12000) {
//         pandian_flag = 5;
//         LOGI("back finish %d", pandian_flag);
//       }

//       break;
//     case 5:
//       if (motor_stop()) {
//         pandian_flag = 6;
//         LOGI("OK %d ", pandian_flag);
//       }

//       break;
//     default:
//       // if (flag == 1) LOGI(" %d ", pandian_flag);
//       break;
//   }
// }