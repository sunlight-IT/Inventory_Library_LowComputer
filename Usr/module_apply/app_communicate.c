#include "app_communicate.h"

#include "communicate/middle_communicate_both.h"
#include "communicate/middle_lower.h"
#include "communicate/middle_upper.h"
#include "component/endianness_change.h"
#include "log/my_log.h"
#include "module_driver/driver_motor.h"
#include "module_middle/middle_book_database.h"
#include "module_middle/middle_event_process.h"

/***************************************

开始盘点以后我才回开始接收下位机的数据信息

1. 说明我必须先接收到上位机的盘点指令
2. 所以这个应该还是上位机的业务逻辑
3. 时间同步也是在盘点开始才同步的


******************************************/

#define REST_LOWER_RECV()    \
  do {                       \
    s_lower_State   = START; \
    s_lower_process = false; \
  } while (0)

#define REST_UPPER_SEND()    \
  do {                       \
    s_upper_State   = START; \
    s_upper_process = false; \
  } while (0)

static CMD_HEAD_IMPROVE_t head_up_cache;
static CMD_HEAD_IMPROVE_t head_low_cache;

static uint8_t* p_low_recv;
static uint8_t* p_up_recv;

static uint8_t* p_low_send;
static uint8_t* p_up_send;

static bool s_lower_process;
static bool s_upper_process;

static int32_t syn_clock;

static uint8_t action_mode = 0xff;

static bool     book_ack;
static uint32_t u32_empty;

void app_communicate_init(void) {
  lower_communite_init();
  upper_communite_init();
}

void app_comunicate(void) {
  if (s_lower_process) {
    // LOGI("TEST1");
    lower_apply_improve();
  }
  if (s_upper_process) {
    // LOGI("TEST21");
    upper_apply_improve();
  }

  low_send();
  upp_send();
}

void lower_apply_improve(void) {
  /*****************/ /*
                    数据处理代码状态机框架
                    1. 空闲
                    2. 输入指令
                    3. 生成指令
                    4. 发送指令
                    5. 等待应答
                    */

  /********************/

  static enum {
    START,
    CMD_ANALYSE,

  } s_lower_State = START;
  static uint32_t time;
  static bool     clock_step;

  switch (s_lower_State) {
    case START:
      // 接收到数据，进入解析状态
      p_low_recv = get_lower_cache();
      memcpy(&head_low_cache, p_low_recv, sizeof(CMD_HEAD_IMPROVE_t));

      for (int i = 0; i < sizeof(CMD_HEAD_IMPROVE_t); i++) {
        LOGI("%02x", p_low_recv[i]);
      }
      // for (int i = 0; i < sizeof(CMD_HEAD_IMPROVE_t) + head_low_cache.len; i++) {
      //   LOGI("%02x", p_low_recv[i]);
      // }

      for (int i = 0; i < head_low_cache.len; i++) {
        LOGI("%02x", p_low_recv[sizeof(CMD_HEAD_IMPROVE_t) + i]);
      }
      s_lower_State = CMD_ANALYSE;
    case CMD_ANALYSE:
      switch (head_low_cache.cmd) {
        case kInit:
        case kInquireBookNum:
        case kInquire:
        case kMoveAction:

          // 2. 再发盘点
          if (head_low_cache.cmd != head_up_cache.cmd) {
            // for (int i = 0; i < sizeof(CMD_HEAD_IMPROVE_t); i++) {
            //   LOGI("%02x", p_low_recv[i]);
            // }
            LOGI("low cmd is %02x", head_low_cache.cmd);
            LOGI("up cmd is %02x", head_up_cache.cmd);
            LOGI("CMD error");
            notify_low_recv_push(0, 0, NULL, kError);  // 接收失败，通知下位机控制块
                                                       // enterQueueEvent(EVENT_LowSend);
            s_lower_State = START;
          }
          // 接收成功，通知下位机控制块
          LOGI("CMD SUCESS");
          notify_low_recv_push(0, 0, NULL, kRecvSucess);
          notify_up_push(head_low_cache.cmd, head_low_cache.len, &p_low_recv[sizeof(CMD_HEAD_IMPROVE_t)], kSend);  // 通知上位机控制块发送

          s_lower_State = START;
          break;
        case kInquireBookInfo:

          Book_Info_Lower_t* p_book_low     = (Book_Info_Lower_t*)&(p_low_recv[sizeof(CMD_HEAD_IMPROVE_t)]);
          Book_Info_Upper_t* p_book_info_up = (Book_Info_Upper_t*)&(p_low_recv[sizeof(CMD_HEAD_IMPROVE_t)]);

          int32_t             time_sec = endianness_change_little(p_book_low->x_time, 4);
          Moter_Config_t*     p_servo  = get_servo_config();
          Book_Info_Ack_Low_t p_back;

          // time_sec /= 1000;

          LOGI("x_time is %08x", p_book_low->x_time);
          LOGI("sec is %d", time_sec);
          LOGI("syn clock is %d", syn_clock);
          LOGI("speed is %d", p_servo->speed);

          p_book_info_up->x_pos = (uint16_t)((time_sec - syn_clock) / UNIT_SPEED_CLOCK_RATIO(p_servo->speed));

          p_book_info_up->y_pox        = p_book_low->y_pox;
          p_book_info_up->reak_freq    = p_book_low->reak_freq;
          p_book_info_up->average_rssi = p_book_low->average_rssi;

          for (int i = 0; i < sizeof(Book_Info_Upper_t); i++) {
            LOGI("%02x", p_low_recv[sizeof(CMD_HEAD_IMPROVE_t) + i]);
          }

          set_lower_book_ack(p_book_low->index);
          input_book_info(p_book_info_up);
          // notify_low_send_push(kInquireBookInfo, 8, &u32_empty, kSendAck);
          book_ack = true;
          //  notify_low_send_push(kInquireBookInfo, 8, &u32_empty, kSendAck);
          s_lower_State = START;
          // LOGI("CMD SUCESS");
          break;
        case kSynClock:
          // LOGI("%08x", (uint32_t*)(&(p_low_recv[sizeof(CMD_HEAD_IMPROVE_t)])));
          // uint8_t* p_clock = (uint8_t*)&syn_clock;
          syn_clock &= 0x00;
          // memcpy(&syn_clock, &p_low_recv[sizeof(CMD_HEAD_IMPROVE_t)], 4);
          syn_clock |= ((uint32_t)p_low_recv[sizeof(CMD_HEAD_IMPROVE_t) + 1]) << 24;
          syn_clock |= ((uint32_t)p_low_recv[sizeof(CMD_HEAD_IMPROVE_t) + 2]) << 16;
          syn_clock |= ((uint32_t)p_low_recv[sizeof(CMD_HEAD_IMPROVE_t) + 3]) << 8;
          syn_clock |= ((uint32_t)p_low_recv[sizeof(CMD_HEAD_IMPROVE_t) + 4]) << 0;
          // syn_clock /= 1000;
          notify_low_recv_push(0, 0, NULL, kRecvSucess);
          LOGI("SYNCLCK is %08x", syn_clock);
          s_lower_State = START;
          break;
      }
      LOGI("CMD ANALYSE");
      s_lower_State = START;
      REST_LOWER_RECV();
      break;
  }
}

// bool move_action_recv_clock(void) {
//   if (head_low_cache.cmd == kSynClock) {
//     notify_low_recv_push(0, 0, NULL, kRecvSucess);
//     return true;
//   }

//   return false;
// }

void upper_apply_improve(void) {
  /*****************/ /*
                    数据处理代码状态机框架
                    1. 空闲
                    2. 输入指令
                    3. 生成指令
                    4. 发送指令
                    5. 等待应答
                    */

  /********************/

  static enum {
    START,
    CMD_ANALYSE,
  } s_upper_State = START;
  static uint32_t time;
  static uint8_t  empty;
  static uint16_t u16_empty;
  switch (s_upper_State) {
    case START:
      // 接收到数据，进入解析状态

      p_up_recv = get_upper_cache();
      memcpy(&head_up_cache, p_up_recv, sizeof(CMD_HEAD_IMPROVE_t));

      for (int i = 0; i < head_up_cache.len; i++) {
        LOGI("%02x", p_up_recv[sizeof(CMD_HEAD_IMPROVE_t) + i]);
      }

      s_upper_State = CMD_ANALYSE;
    case CMD_ANALYSE:

      // if (get_servo_config_reset_state() != 0 && head_up_cache.cmd !=) {
      //   break;
      // }

      switch (head_up_cache.cmd) {
        case kInit:
        case kInquireBookNum:
          // 消息队列机制还需要修改
          notify_low_send_push(head_up_cache.cmd, head_up_cache.len, &(p_up_recv[sizeof(CMD_HEAD_IMPROVE_t)]), kSend);

          break;
        case kMoveAction:
          Move_Action_t* p_action = (Move_Action_t*)(&(p_up_recv[sizeof(CMD_HEAD_IMPROVE_t)]));  // 1 == 空闲数据位

          action_mode = p_action->action_state;  // 记录盘点

          servo_config_updata(kPos, endianness_change_little(p_action->x_pos, 2));      // 记录位置
          servo_config_updata(kSpeed, endianness_change_little(p_action->x_speed, 2));  // 记录速度
          servo_config_updata(kActionMode, kMoveAction);
          // LOGI("action is %d", action_mode);

          notify_low_send_push(kSynClock, 4, &empty, kSend);
          notify_low_send_push(head_up_cache.cmd, head_up_cache.len, &(p_up_recv[sizeof(CMD_HEAD_IMPROVE_t)]), kSend);
          break;
        case kResetPos:
          uint8_t reset_mode = 0;
          reset_mode         = p_up_recv[sizeof(CMD_HEAD_IMPROVE_t) + 1];
          // Reset_Mode_t* p_reset = (Reset_Mode_t*)(&(p_up_recv[sizeof(CMD_HEAD_IMPROVE_t)]));
          LOGI("reset is %d", reset_mode);

          servo_config_updata(kActionMode, kResetPos);  // 复位模式
                                                        // servo_config_updata(kPos, 0);                 // 记录目标位置
          servo_config_updata(kResetMode, reset_mode);  // 记录手动或自动复位

          // servo_config_updata(kSpeed, endianness_change_little(p_action->x_speed, 2));  // 记录速度
          break;
        case kInquire:
          // 消息队列机制还需要修改
          notify_low_send_push(head_up_cache.cmd, head_up_cache.len, &(p_up_recv[sizeof(CMD_HEAD_IMPROVE_t)]), kSend);
          break;
        case kInquireBookInfo:
          // Book_Info_Upper_t* p_book = (uint8_t*)&(p_low_recv[sizeof(CMD_HEAD_IMPROVE_t)]);
          uint32_t index = 0;

          Book_Info_Lower_t* p_book_dabase;
          // LOGI("%02x", p_up_recv[sizeof(CMD_HEAD_IMPROVE_t) + 1]);
          // LOGI("%02x", p_up_recv[sizeof(CMD_HEAD_IMPROVE_t) + 2]);
          // LOGI("%02x", p_up_recv[sizeof(CMD_HEAD_IMPROVE_t) + 3]);
          // LOGI("%02x", p_up_recv[sizeof(CMD_HEAD_IMPROVE_t) + 4]);
          index |= p_up_recv[sizeof(CMD_HEAD_IMPROVE_t) + 1] << 24;  //*((uint32_t*)&(p_up_recv[sizeof(CMD_HEAD_IMPROVE_t)]));
          index |= p_up_recv[sizeof(CMD_HEAD_IMPROVE_t) + 2] << 16;
          index |= p_up_recv[sizeof(CMD_HEAD_IMPROVE_t) + 3] << 8;
          index |= p_up_recv[sizeof(CMD_HEAD_IMPROVE_t) + 4] << 0;
          LOGI("finding index %04x", index);
          p_book_dabase = output_book_info(index);

          if (p_book_dabase == NULL)
            notify_up_push(head_up_cache.cmd, 0, NULL, kBookEmpty);
          else
            notify_up_push(head_up_cache.cmd, sizeof(Book_Info_Lower_t) + 3, p_book_dabase, kSend);
          break;
      }
      LOGI("CMD ANALYSE");
      s_upper_State = START;
      REST_UPPER_SEND();
      break;
  }
}

void book_ack_func(void) {
  if (book_ack) {
    notify_low_send_push(kInquireBookInfo, 8, &u32_empty, kSendAck);
    book_ack = false;
  }
}

void move_action_flow(void) {
  /*
    1.同步时间
    2.运动到指定位置
    3.开始盘点
    4.接收图书数据
  */

  static enum {
    START,
    SYN_CLOCK,
    NON_CHECK_ACTION,
    CHECK_ACTION,
    RECV_BOOK_INFO,
    WAIT_SUCESS,
  } s_action_State = START;

  switch (s_action_State) {
    case START:
      break;
    case SYN_CLOCK:
      notify_low_push(head_up_cache.cmd, head_up_cache.len, &(p_up_recv[sizeof(CMD_HEAD_IMPROVE_t)]), kSend);
      break;
    case NON_CHECK_ACTION:
      break;
    case CHECK_ACTION:
      break;
    case RECV_BOOK_INFO:
      break;
    case WAIT_SUCESS:
      break;
  }
}

void init_flow(void) {
  /*
    1.发信号给下位机
    2.发信号给上位机
  */

  static enum {
    START,
    SEND_LOW,
    SEND_UP,
  } s_init_State = START;
}

void book_info_flow(void) {
  /*
    1.发信号给下位机
    2.发信号给上位机
  */
}

void    set_upper_process(bool state) { s_upper_process = state; }
void    set_lower_process(bool state) { s_lower_process = state; }
uint8_t get_action_mode(void) { return action_mode; };

uint32_t get_syn_time_data(void) { return syn_clock; }