#include "middle_upper.h"

#include "ServoMotor.h"
#include "component/endianness_change.h"
#include "log/my_log.h"
#include "module_driver/driver_motor.h"
#include "module_middle/middle_event_process.h"

static uint8_t send_buf[SEND_BUF_LEN];
static uint8_t send_len;

static uint8_t up_recv_buf[SEND_BUF_LEN];
static uint8_t up_recv_len;

static Notion_t notify_up_queue[QUEUE_NUM];
static uint8_t  notify_up_head = 0;
static uint8_t  notify_up_tail = 0;

static CMD_HEAD_IMPROVE_t head_up_cache;
static CMD_HEAD_IMPROVE_t head_low_cache;

static uint32_t low_clock;

static uint8_t empty;

void upper_communite_init(void) { registCallback(EVENT_UpSend, upp_send); }  // 只能运行一次所以pass掉

static uint16_t CRC16_Calculate(uint8_t* data, uint8_t len) {
  uint16_t crc16 = 0xffff;
  uint16_t temp  = 0;
  for (int i = 0; i < len; i++) {
    crc16 ^= (uint16_t)data[i];
    for (int j = 0; j < 8; j++) {
      if (crc16 & 0x0001) {
        crc16 = (crc16 >> 1) ^ 0xa001;
      } else {
        crc16 = (crc16 >> 1);
      }
    }
  }
  LOGI("CRC is :%04x", crc16);
  return crc16;
}

void notify_up_push(uint8_t cmd, uint8_t len, uint8_t* p_data, LOWER_NOTION notion) {
  if ((notify_up_head + 1) % QUEUE_NUM == notify_up_tail) {
    LOGW("notify up queue full");
    return;
  }

  Notion_t* p_notion = &notify_up_queue[notify_up_head];
  p_notion->cmd_head = cmd;
  // memcpy(&(p_notion->cmd_head), p_cmd_head, sizeof(CMD_HEAD_IMPROVE_t));
  if (len != 0) {
    memcpy(p_notion->data_regeion, p_data, len - 3);  // 3 == 1 + 2  1 == 帧尾  2 == crc16;
    p_notion->len = len;
  }

  p_notion->state = notion;
  notify_up_head  = (notify_up_head + 1) % QUEUE_NUM;
  LOGW("upper push success");
}

Notion_t notify_up_pop(void) {
  Notion_t notion = {.state = kEmpty};
  if (notify_up_tail == notify_up_head) {
    // LOGW("notify up queue empty");
    return notion;
  }

  notion         = notify_up_queue[notify_up_tail];
  notify_up_tail = (notify_up_tail + 1) % QUEUE_NUM;
  LOGW("notify pop success state is : %02x", notion.state);
  return notion;
}

uint8_t data_pack_upper(ENUM_CMD_UPPER_t type, uint8_t* data_region) {
  uint8_t data_size;
  if (data_region == NULL) {
    LOGE("data_region is null");
    return;
  }
  switch (type) {
    case kInit:
      data_size = sizeof(Upper_Init_t);
      init_data_pack(data_region);
      break;
    case kMoveAction:
      data_size = sizeof(Move_Action_t);
      move_data_pack(data_region);
      break;

    case kInquireBookInfo:
      data_size = sizeof(Book_Info_Upper_t);
      book_info_pack(data_region);
      break;
    case kInquireBookNum:
      data_size = 5;
      break;
    case kInquire:
      data_size = sizeof(Move_Action_t);
      move_data_pack(data_region);
      break;
    case kResetPos:
      data_size = 2;
      break;
    case kSynClock:
      break;
  }

  return data_size;
}

void send_upper(ENUM_CMD_UPPER_t cmd, uint8_t tail_len, uint8_t* data) {
  uint8_t data_len;
  // uint8_t data_temp[DATA_REGION_SIZE];
  // memcpy(data_temp, up_recv_buf + sizeof(CMD_HEAD_t), up_recv_len - sizeof(CMD_HEAD_t) - 3);
  //  for (int i = 0; i < up_recv_len - 6 - 3; i++) {
  //    LOGI("%02x", ((uint8_t*)data_temp)[i]);
  //  }
  data_len = data_pack_upper(cmd, data);
  _send_upper(cmd, tail_len, data, data_len);
}

/// @brief send to upper computer message
/// @param cmd
/// @param tail_len  message length = head_len + tail_len
/// @param data     data region
/// @param data_len data region size
void _send_upper(ENUM_CMD_UPPER_t cmd, uint8_t tail_len, void* data, uint8_t data_size) {
  CMD_HEAD_IMPROVE_t* p_head   = (CMD_HEAD_IMPROVE_t*)send_buf;
  uint8_t             head_len = sizeof(CMD_HEAD_IMPROVE_t);
  uint16_t            crc16;
  p_head->head   = 0x70;
  p_head->cmd    = (uint8_t)cmd;
  p_head->encord = 0x01;
  p_head->addr   = 0x0a;
  p_head->len    = tail_len;
  //   p_head->empty  = 0x00;

  LOGI("tail_len is %02x", tail_len);
  // data region copy
  memcpy(&send_buf[head_len], (uint8_t*)data, data_size);
  send_len = head_len + data_size;

  send_buf[send_len] = 0x71;
  send_len += 1;

  crc16 = CRC16_Calculate(send_buf, send_len);
  memcpy(&send_buf[send_len], &crc16, sizeof(uint16_t));
  send_len += 2;
  for (int i = 0; i < send_len; i++)  //
    LOGI("%02x", send_buf[i]);
  // wireless_send(send_buf, send_len);

  HAL_UART_Transmit(&huart4, send_buf, send_len, 100);
}

void move_data_pack(uint8_t* data_region) {
  Motor_t*       servo_info = get_servo_info();
  Move_Action_t* p_action   = (Move_Action_t*)data_region;

  p_action->x_pos       = get_Position_mm(GetMoterStateReg(), 4);
  p_action->x_speed     = servo_info->speed;
  p_action->motor_state = 0xa1;
  p_action->direct      = servo_info->diretion;
  // p_action->y_pos           = 0x0001;
  // p_action->y_speed         = 0x0001;
  // p_action->action_state    = 0x01;

  // action_mode = p_action->action_state;
  for (int i = 0; i < sizeof(Move_Action_t); i++) {
    LOGI("%02x", data_region[i]);
  }
}

void book_num_pack(uint8_t* data_region) { uint32_t* p_num = (uint32_t*)data_region; }

void book_info_pack(uint8_t* data_region) {
  Book_Info_Upper_t* p_book_info_low = (Book_Info_Lower_t*)data_region;
  // Book_Info_Upper_t* p_book_info_up  = (Book_Info_Upper_t*)data_region;
  Motor_t* servo = get_servo_info();
  // for (int i = 0; i < sizeof(Book_Info_Lower_t); i++) {
  //   LOGI("%02x", data_region[i]);
  // }
  // uint32_t time_sec = endianness_change_little(p_book_info_low->x_time, 4);
  //  LOGI("x_time is %08x", p_book_info_low->x_time);
  //  LOGI("sec is %d", time_sec);
  //  LOGI("clock is %04x", low_clock);
  //  LOGI("speed is %d", servo->speed);
  //  p_book_info_up->x_pos = (uint16_t)(servo->speed * (time_sec - low_clock));

  // p_book_info_up->y_pox        = p_book_info_low->y_pox;
  // p_book_info_up->reak_freq    = p_book_info_low->reak_freq;
  // p_book_info_up->average_rssi = p_book_info_low->average_rssi;

  for (int i = 0; i < sizeof(Book_Info_Upper_t); i++) {
    LOGI("%02x", data_region[i]);
  }
}

void init_data_pack(uint8_t* data_region) {
  Motor_t*      servo_info  = get_servo_info();
  Upper_Init_t* p_init      = (Upper_Init_t*)data_region;
  p_init->servo_conect      = servo_info->connect;
  p_init->servo_reset_state = servo_info->state;
}

// void upper_apply_improve(void) {
//   /*****************/ /*
//                     数据处理代码状态机框架
//                     1. 空闲
//                     2. 输入指令
//                     3. 生成指令
//                     4. 发送指令
//                     5. 等待应答
//                     */

//   /********************/

//   static enum {
//     START,
//     CMD_ANALYSE,
//   } s_upper_State = START;
//   static uint32_t time;

//   switch (s_upper_State) {
//     case START:
//       // 接收到数据，进入解析状态

//       memcpy(&head_up, up_recv_buf, sizeof(CMD_HEAD_IMPROVE_t));
//       s_upper_State = CMD_ANALYSE;
//     case CMD_ANALYSE:

//       switch (head_up.cmd) {
//         case kInit:
//         case kInquireBookNum:
//         case kInquireBookInfo:
//         case kInquire:
//           notify_low_push(head_up.len, head_up.cmd, kSend);
//           break;
//         case kMoveAction:
//           Motor_t*       servo_set = get_servo_set_info();
//           Move_Action_t* p_action  = (Move_Action_t*)(&(up_recv_buf[sizeof(CMD_HEAD_IMPROVE_t) + 1]));  // 1 == 空闲数据位
//           servo_set->pos           = p_action->x_pos;
//           action_mode              = p_action->action_state;
//           notify_low_push(head_up.len, head_up.cmd, kSend);
//           break;
//       }
//       LOGI("CMD ANALYSE");
//       s_upper_State = START;
//       REST_UPPER_SEND();
//       break;
//   }
// }

void upp_send(void) {
  static enum {
    START,
    SEND,
    WAIT,
    SUCCESS,
    ERROR,
    TIMEOUT,
  } s_State = START;

  static uint32_t time;
  Notion_t        notion;

  switch (s_State) {
    case START:
      notion  = notify_up_pop();
      s_State = SEND;
    case SEND:
      if (notion.state == kEmpty) {
        // LOGI("notion is empty");
        s_State = START;
      } else if (notion.state == kBookEmpty) {
        _send_upper(notion.cmd_head, 4, &empty, 1);
        // send_upper(notion.cmd_head, notion.len, notion.data_regeion);
        // time    = HAL_GetTick();
        s_State = START;
      } else if (notion.state == kSend) {
        send_upper(notion.cmd_head, notion.len, notion.data_regeion);
        time    = HAL_GetTick();
        s_State = START;
      }
      break;
    case WAIT:

      if (has_pass_time(time) > WAIT_RECV_TIMEOUT) {
        LOGI("timeout");
        s_State = START;
        break;
      }

      notion = notify_up_pop();
      if (notion.state == kError) {
        s_State = ERROR;
        break;
      } else if (notion.state == kRecvSucess) {
        LOGI("success");
        s_State = SUCCESS;
        break;
      }

      s_State = WAIT;
      break;
    case SUCCESS:
      LOGI("success");
      s_State = START;
      break;
    case TIMEOUT:
      LOGI("timeout");
      s_State = START;
      break;
  }
}

uint8_t* get_upper_cache(void) { return up_recv_buf; }
void     set_upper_recv_len(uint8_t len) { up_recv_len = len; }
