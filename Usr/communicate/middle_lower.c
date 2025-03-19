#include "middle_lower.h"

#include <string.h>

#include "log/my_log.h"
#include "module_driver/driver_motor.h"
#include "module_driver/driver_wireless.h"
#include "module_middle/middle_event_process.h"

static Notion_t notify_low_queue_send[QUEUE_NUM];
static uint8_t  notify_low_send_head = 0;
static uint8_t  notify_low_send_tail = 0;

static Notion_t notify_low_queue_recv[QUEUE_NUM];
static uint8_t  notify_low_recv_head = 0;
static uint8_t  notify_low_recv_tail = 0;

static uint8_t low_recv_buf[SEND_BUF_LEN];
static uint8_t low_recv_len;

static uint8_t send_buf[SEND_BUF_LEN];
static uint8_t send_len;

static CMD_HEAD_IMPROVE_t head_up_cache;
static CMD_HEAD_IMPROVE_t head_low_cache;

static uint32_t book_index_ack;

void lower_communite_init(void) { registCallback(EVENT_LowSend, low_send); }

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

void notify_low_send_push(uint8_t cmd, uint8_t len, uint8_t* p_data, LOWER_NOTION notion) {
  if ((notify_low_send_head + 1) % QUEUE_NUM == notify_low_send_tail) {
    LOGW("notify low queue full");
    return;
  }
  Notion_t* p_notion = &notify_low_queue_send[notify_low_send_head];

  p_notion->cmd_head = cmd;

  if (len != 0) {
    memcpy(p_notion->data_regeion, p_data, len - 3);  // 3 == 1 + 2  1 == 帧尾  2 == crc16;
    p_notion->len = len;
    LOGI("len is %02x", len);
  }
  p_notion->state      = notion;
  notify_low_send_head = (notify_low_send_head + 1) % QUEUE_NUM;

  // for (int i = 0; i < len; i++) {
  //   LOGI("%02x", p_data[i]);
  // }
  LOGI("low push success cmd %02x", p_notion->cmd_head);
}

Notion_t notify_low_send_pop(void) {
  Notion_t notion = {.state = kEmpty};
  if (notify_low_send_tail == notify_low_send_head) {
    // LOGW("notify low queue empty");
    return notion;
  }

  notion               = notify_low_queue_send[notify_low_send_tail];
  notify_low_send_tail = (notify_low_send_tail + 1) % QUEUE_NUM;

  LOGW("notify pop success state is : %02x", notion.state);
  return notion;
}

void notify_low_recv_push(uint8_t cmd, uint8_t len, uint8_t* p_data, LOWER_NOTION notion) {
  if ((notify_low_recv_head + 1) % QUEUE_NUM == notify_low_recv_tail) {
    LOGW("notify low queue full");
    return;
  }
  Notion_t* p_notion = &notify_low_queue_recv[notify_low_recv_head];

  p_notion->cmd_head = cmd;

  if (len != 0) {
    memcpy(p_notion->data_regeion, p_data, len - 3);  // 3 == 1 + 2  1 == 帧尾  2 == crc16;
    p_notion->len = len;
  }
  p_notion->state      = notion;
  notify_low_recv_head = (notify_low_recv_head + 1) % QUEUE_NUM;

  // for (int i = 0; i < len; i++) {
  //   LOGI("%02x", p_data[i]);
  // }
  LOGI("low recv push success cmd %02x", p_notion->cmd_head);
}

Notion_t notify_low_recv_pop(void) {
  Notion_t notion = {.state = kEmpty};
  if (notify_low_recv_tail == notify_low_recv_head) {
    // LOGW("notify low queue empty");
    return notion;
  }

  notion               = notify_low_queue_recv[notify_low_recv_tail];
  notify_low_recv_tail = (notify_low_recv_tail + 1) % QUEUE_NUM;

  LOGW("notify pop success state is : %02x", notion.state);
  return notion;
}

/// @brief send to lower computer message
/// @param cmd
/// @param tail_len  message length = head_len + tail_len
/// @param data     data region
/// @param data_len data region size
void _send_lower(ENUM_CMD_UPPER_t cmd, uint8_t tail_len, void* data, uint8_t data_size) {
  CMD_HEAD_IMPROVE_t* p_head   = (CMD_HEAD_IMPROVE_t*)send_buf;
  uint8_t             head_len = sizeof(CMD_HEAD_IMPROVE_t);
  uint16_t            crc16;
  p_head->head   = 0xd0;
  p_head->cmd    = (uint8_t)cmd;
  p_head->encord = 0x01;
  p_head->addr   = 0x0b;
  p_head->len    = tail_len;
  // p_head->empty  = 0x00;

  for (int i = 0; i < sizeof(CMD_HEAD_IMPROVE_t); i++) {
    LOGI("%02x", send_buf[i]);
  }

  memcpy(&send_buf[head_len], (uint8_t*)data, data_size);
  send_len           = head_len + data_size;
  send_buf[send_len] = 0xd1;
  send_len += 1;

  crc16 = CRC16_Calculate(send_buf, send_len);
  memcpy(&send_buf[send_len], &crc16, sizeof(uint16_t));
  send_len += 2;
  for (int i = 0; i < send_len; i++)  //
    LOGI("%02x", send_buf[i]);

  // 数据发送
  wireless_send(send_buf, send_len);
}

void move_data_pack_low(uint8_t* data_region) {
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

void book_info_pack_low(uint8_t* data_region) {
  Book_Info_Ack_Low_t* p_book_info_back = (Book_Info_Ack_Low_t*)data_region;
  p_book_info_back->empty               = 0x01;
  p_book_info_back->index_book          = book_index_ack;
}

uint8_t data_pack_lower(ENUM_CMD_UPPER_t type, uint8_t* data_region) {
  uint8_t data_size;
  if (data_region == NULL) {
    LOGE("data_region is null");
    return;
  }
  switch (type) {
    case kSynClock:
      data_size = 1;
      break;
    case kInit:
    case kInquire:
      data_size = 1;
      break;
    case kInquireBookInfo:
      data_size = 5;
      book_info_pack_low(data_region);
      break;
    case kInquireBookNum:
      data_size = 4;
      break;
    case kMoveAction:
      data_size = sizeof(Move_Action_t);
      move_data_pack_low(data_region);
      break;
  }
  return data_size;
}

void send_lower(ENUM_CMD_UPPER_t cmd, uint8_t tail_len, uint8_t* data) {
  uint8_t data_len;
  // LOGI("TAIL LEN is %02x", tail_len);
  // memcpy(data_temp, up_recv_buf + sizeof(CMD_HEAD_t), up_recv_len - sizeof(CMD_HEAD_t) - 3);
  // for (int i = 0; i < tail_len; i++) {
  //   LOGI("%02X", data[i]);
  // }
  data_len = data_pack_lower(cmd, data);
  _send_lower(cmd, tail_len, data, data_len);
}

void low_send(void) {
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
      notion  = notify_low_send_pop();
      s_State = SEND;
    case SEND:
      if (notion.state == kEmpty) {
        // LOGI("notion is empty");
        s_State = START;
      } else if (notion.state == kError) {
        LOGI("notion is error");
        s_State = START;
      } else if (notion.state == kSend) {
        LOGI("notion is SEND");
        LOGI("notion is %2x", notion.len);

        // for (int i = 0; i < notion.len; i++) {
        //   LOGI("%02x", notion.data_regeion[i]);
        // }
        send_lower(notion.cmd_head, notion.len, notion.data_regeion);
        time    = HAL_GetTick();
        s_State = WAIT;
      } else if (kSendAck) {
        send_lower(notion.cmd_head, notion.len, notion.data_regeion);
        s_State = START;
      }
      break;
    case WAIT:

      if (has_pass_time(time) > WAIT_RECV_TIMEOUT) {
        // LOGI("timeout");
        s_State = TIMEOUT;
        break;
      }

      notion = notify_low_recv_pop();
      if (notion.state == kError) {
        s_State = ERROR;
        break;
      } else if (notion.state == kRecvSucess) {
        // LOGI("success");
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
      s_State = SEND;
      break;
  }
}

uint8_t* get_lower_cache(void) { return low_recv_buf; }

uint8_t* get_lower_send_cache(void) { return send_buf; }

void set_lower_send_len(uint8_t len) { send_len = len; }
void set_lower_recv_len(uint8_t len) { low_recv_len = len; }

void set_lower_book_ack(uint32_t book_index) { book_index_ack = book_index; }