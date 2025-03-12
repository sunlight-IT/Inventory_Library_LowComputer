#include "cmd_process.h"

#include <string.h>

#include "ServoMotor.h"
#include "log/my_log.h"
#include "module_driver/driver_motor.h"
#include "module_middle/middle_fsm.h"

typedef struct {
  uint8_t* data;
  uint8_t  size;
} Data_Region_t;

typedef struct __attribute__((packed)) {
  uint16_t x_pos;
  uint16_t x_speed;
  uint16_t y_pos;
  uint16_t y_speed;
  uint8_t  action_state;
  uint8_t  direct;
  uint8_t  motor_state;
} Move_Action_t;

typedef struct __attribute__((packed)) {
  uint8_t servo_conect;
  uint8_t servo_reset_state;
  uint8_t check_machine_connect;
  uint8_t check_machine_reset;
  uint8_t emergency_stop;
  uint8_t reader_connect;
  uint8_t radar_connect;
} Upper_Init_t;

typedef struct __attribute__((packed)) {
  uint8_t  index[3];
  uint8_t  UID[8];
  uint8_t  RSSID;
  uint32_t x_time;
  uint16_t y_pox;
  uint8_t  reak_freq;
  uint8_t  average_rssi;
} Book_Info_Lower_t;

typedef struct __attribute__((packed)) {
  uint8_t  index[3];
  uint8_t  UID[8];
  uint8_t  RSSID;
  uint16_t x_pos;
  uint16_t y_pox;
  uint8_t  reak_freq;
  uint8_t  average_rssi;
} Book_Info_Upper_t;

static uint8_t send_buf[SEND_BUF_LEN];
static uint8_t send_len;

static uint8_t recv_buf[SEND_BUF_LEN];
static uint8_t recv_len;

static uint8_t up_recv_buf[SEND_BUF_LEN];
static uint8_t up_recv_len;
static bool    upper_start;
static bool    up_state;

static uint8_t low_recv_buf[SEND_BUF_LEN];
static uint8_t low_recv_len;
static bool    low_start;
static bool    low_state;

static uint32_t low_clock;

static bool user_power;

static uint8_t action_mode = 0xff;

static uint8_t data_pack_lower(ENUM_CMD_UPPER_t type, uint8_t* data_region);
static uint8_t data_pack_upper(ENUM_CMD_UPPER_t type, uint8_t* data_region);

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

void send_lower(ENUM_CMD_UPPER_t cmd, uint8_t tail_len) {
  uint8_t data_len;
  uint8_t data_temp[DATA_REGION_SIZE];

  memcpy(data_temp, up_recv_buf + sizeof(CMD_HEAD_t), up_recv_len - sizeof(CMD_HEAD_t) - 3);
  data_len = data_pack_lower(cmd, data_temp);
  _send_lower(cmd, tail_len, data_temp, data_len);
}

void send_upper(ENUM_CMD_UPPER_t cmd, uint8_t tail_len) {
  uint8_t data_len;
  uint8_t data_temp[DATA_REGION_SIZE];
  memcpy(data_temp, up_recv_buf + sizeof(CMD_HEAD_t), up_recv_len - 6 - 3);
  for (int i = 0; i < up_recv_len - 6 - 3; i++) {
    LOGI("%02x", ((uint8_t*)data_temp)[i]);
  }
  data_len = data_pack_upper(cmd, data_temp);
  _send_upper(cmd, tail_len, data_temp, data_len);
}

/// @brief send to lower computer message
/// @param cmd
/// @param tail_len  message length = head_len + tail_len
/// @param data     data region
/// @param data_len data region size
void _send_lower(ENUM_CMD_UPPER_t cmd, uint8_t tail_len, void* data, uint8_t data_size) {
  CMD_HEAD_t* p_head   = (CMD_HEAD_t*)send_buf;
  uint8_t     head_len = sizeof(CMD_HEAD_t);
  uint16_t    crc16;
  p_head->head   = 0xd0;
  p_head->cmd    = (uint8_t)cmd;
  p_head->encord = 0x01;
  p_head->addr   = 0x0b;
  p_head->len    = tail_len;
  p_head->empty  = 0x00;

  for (int i = 0; i < sizeof(CMD_HEAD_t); i++) {
    LOGI("%02x", send_buf[i]);
  }

  // data region copy
  // for (int i = 0; i < data_size; i++) {
  //   LOGI("%02x", ((uint8_t*)data)[i]);
  // }
  memcpy(&send_buf[head_len], (uint8_t*)data, data_size);
  send_len           = head_len + data_size;
  send_buf[send_len] = 0xd1;
  send_len += 1;

  crc16 = CRC16_Calculate(send_buf, send_len);
  memcpy(&send_buf[send_len], &crc16, sizeof(uint16_t));
  send_len += 2;
  for (int i = 0; i < send_len; i++)  //
    LOGI("%02x", send_buf[i]);
  wireless_send(send_buf, send_len);
}

/// @brief send to upper computer message
/// @param cmd
/// @param tail_len  message length = head_len + tail_len
/// @param data     data region
/// @param data_len data region size
void _send_upper(ENUM_CMD_UPPER_t cmd, uint8_t tail_len, void* data, uint8_t data_size) {
  CMD_HEAD_t* p_head   = (CMD_HEAD_t*)send_buf;
  uint8_t     head_len = sizeof(CMD_HEAD_t);
  uint16_t    crc16;
  p_head->head   = 0x70;
  p_head->cmd    = (uint8_t)cmd;
  p_head->encord = 0x01;
  p_head->addr   = 0x0a;
  p_head->len    = tail_len;
  p_head->empty  = 0x00;

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

uint8_t data_pack_lower(ENUM_CMD_UPPER_t type, uint8_t* data_region) {
  uint8_t data_size;
  if (data_region == NULL) {
    LOGE("data_region is null");
    return;
  }
  switch (type) {
    case kInit:

    case kInquire:

      data_size = 1;
      break;
    case kInquireBookInfo:
    case kInquireBookNum:
      data_size = 4;
      break;
    case kSynClock:
      data_size = 0;
      break;
    case kMoveAction:
      data_size = sizeof(Move_Action_t);
      move_data_pack(data_region);
      break;
  }
  return data_size;
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
      data_size = 4;
      break;
    case kInquire:
      data_size = sizeof(Move_Action_t);
      move_data_pack(data_region);
      break;
    case kSynClock:
      break;
  }

  return data_size;
}

void move_data_pack(uint8_t* data_region) {
  Motor_t        servo_info = get_servo_info();
  Move_Action_t* p_action   = (Move_Action_t*)data_region;

  p_action->x_pos       = get_Position_mm(GetMoterStateReg(), 4);
  p_action->x_speed     = servo_info.speed;
  p_action->motor_state = 0xa1;
  p_action->direct      = servo_info.diretion;
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
  Book_Info_Lower_t* p_book_info_low = (Book_Info_Lower_t*)data_region;
  Book_Info_Upper_t* p_book_info_up  = (Book_Info_Upper_t*)data_region;
  Motor_t            servo           = get_servo_info();
  uint32_t           time_sec        = p_book_info_low->x_time / 1000;
  LOGI("x_time is %08x", p_book_info_low->x_time);
  LOGI("sec is %d", time_sec);
  LOGI("clock is %04x", low_clock);
  LOGI("speed is %d", servo.speed);
  p_book_info_up->x_pos = (uint16_t)(servo.speed * (time_sec - low_clock));

  p_book_info_up->y_pox        = p_book_info_low->y_pox;
  p_book_info_up->reak_freq    = p_book_info_low->reak_freq;
  p_book_info_up->average_rssi = p_book_info_low->average_rssi;

  for (int i = 0; i < sizeof(Book_Info_Upper_t); i++) {
    LOGI("%02x", data_region[i]);
  }
}

void init_data_pack(uint8_t* data_region) {
  Motor_t       servo_info  = get_servo_info();
  Upper_Init_t* p_init      = (Upper_Init_t*)data_region;
  p_init->servo_conect      = servo_info.connect;
  p_init->servo_reset_state = servo_info.state;
}

FSM_STATE_t init_upper(uint8_t len) {
  /********流程********/
  /*
  1.
  2.
  3. 发送下位机初始化
  4.
  5. 发送数据给上位机
  */

  static enum {
    START,
    SEND_INIT_LOW,
    SEND_UP,
    WAITE_LOW,
  } s_State = START;

  static uint32_t up_flow;
  switch (s_State) {
    case START:
      s_State = SEND_INIT_LOW;
      break;
    case SEND_INIT_LOW:
      send_lower(kInit, len);

      low_state = false;
      up_flow   = SEND_INIT_LOW;

      s_State = WAITE_LOW;
      LOGE("LOW FALSE");
      break;
    case SEND_UP:
      send_upper(kInit, len);

      low_state = false;
      up_flow   = START;

      s_State = WAITE_LOW;
      return fsm_cpl;
      break;
    case WAITE_LOW:
      if (true == low_state) {
        LOGE("LOW TRUE");
        switch (up_flow) {
          case SEND_INIT_LOW:
            s_State = SEND_UP;
        }
      } else
        s_State = WAITE_LOW;
      break;
  }

  return fsm_onging;
}

FSM_STATE_t inquire_upper(uint8_t len) {
  /********流程********/
  /*
  1.
  2.
  3. 发送下位机查询
  4.
  5. 发送数据给上位机
  */
  static enum {
    START,
    SEND_START_LOW,
    SEND_UP,
    WAITE_LOW,
  } s_State = START;

  static uint32_t up_flow;
  switch (s_State) {
    case START:
      s_State = SEND_START_LOW;
      break;
    case SEND_START_LOW:
      send_lower(kInquire, len);

      low_state = false;
      up_flow   = SEND_START_LOW;
      s_State   = WAITE_LOW;

      break;
    case SEND_UP:
      send_upper(kInit, len);
      low_state = false;
      up_flow   = SEND_UP;
      s_State   = START;
      return fsm_cpl;
      break;
    case WAITE_LOW:
      if (true == low_state) {
        LOGE("LOW TRUE");
        switch (up_flow) {
          case SEND_START_LOW:
            s_State = SEND_UP;
        }
      } else
        s_State = WAITE_LOW;
      break;
  }

  return fsm_onging;
}

FSM_STATE_t move_action_upper(uint8_t len) {  // uint8_t* data, uint8_t len) {
  /********流程********/
  /*
  1. 发给下位机同步时间
  2.
  3. 发送下位机开始盘点
  4.
  5. 发送数据给上位机
  */
  static enum {
    START,
    SEND_START_LOW,
    SEND_UP,
    WAITE_LOW,
  } s_State = START;

  static uint32_t up_flow;
  switch (s_State) {
    case START:
      s_State = SEND_START_LOW;

    case SEND_START_LOW:
      LOGE("send start");
      send_lower(kMoveAction, len);

      low_state = false;
      up_flow   = SEND_START_LOW;
      s_State   = WAITE_LOW;
      break;
    case SEND_UP:
      LOGE("send up");
      send_upper(kMoveAction, len);

      low_state = false;
      up_flow   = SEND_UP;
      s_State   = START;

      return fsm_cpl;
      break;
    case WAITE_LOW:
      // LOGE("wAIT");
      if (true == low_state) {
        switch (up_flow) {
          case SEND_START_LOW:
            s_State = SEND_UP;
            break;
        }
      } else

        s_State = WAITE_LOW;
      break;
  }

  return fsm_onging;
}

FSM_STATE_t book_info_upper(void) {
  /********流程********/
  /*
  1. 发送图书读取信号给下位机
  2. 发送图书信号给上位机
  */

  static enum {
    START,
    SEND_START_LOW,
    SEND_UP,
    WAITE_LOW,
  } s_State = START;

  static uint32_t up_flow;
  switch (s_State) {
    case START:
      s_State = SEND_START_LOW;

    case SEND_START_LOW:
      LOGE("send start");
      send_lower(kInquireBookInfo, 4 + 3 + 1);  // 4 == 数据序列， 3 == crc + 帧尾， 1 == 数据区预留位

      low_state = false;
      up_flow   = SEND_START_LOW;
      s_State   = WAITE_LOW;
      break;
    case SEND_UP:
      LOGE("send up");
      send_upper(kInquireBookInfo, sizeof(Book_Info_Upper_t) + 3 + 1);

      low_state = false;
      up_flow   = SEND_UP;
      s_State   = START;

      return fsm_cpl;
      break;
    case WAITE_LOW:
      // LOGE("wAIT");
      if (true == low_state) {
        switch (up_flow) {
          case SEND_START_LOW:
            s_State = SEND_UP;
            break;
        }
      } else

        s_State = WAITE_LOW;
      break;
  }

  return fsm_onging;
}

FSM_STATE_t book_num_upper(void) {
  /********流程********/
  /*
  1. 发送图书读取信号给下位机
  2. 发送图书信号给上位机
  */

  static enum {
    START,
    SEND_START_LOW,
    SEND_UP,
    WAITE_LOW,
  } s_State = START;

  static uint32_t up_flow;
  switch (s_State) {
    case START:
      s_State = SEND_START_LOW;

    case SEND_START_LOW:
      LOGE("send start");
      send_lower(kInquireBookNum, 4 + 3 + 1);  // 4 == 数据序列， 3 == crc + 帧尾， 1 == 数据区预留位

      low_state = false;
      up_flow   = SEND_START_LOW;
      s_State   = WAITE_LOW;
      break;
    case SEND_UP:
      LOGE("send up");
      send_upper(kInquireBookNum, 4 + 3 + 1);

      low_state = false;
      up_flow   = SEND_UP;
      s_State   = START;

      return fsm_cpl;
      break;
    case WAITE_LOW:
      // LOGE("wAIT");
      if (true == low_state) {
        switch (up_flow) {
          case SEND_START_LOW:
            s_State = SEND_UP;
            break;
        }
      } else

        s_State = WAITE_LOW;
      break;
  }

  return fsm_onging;
}

FSM_STATE_t syn_clock_upper(void) {
  static enum {
    START,
    SEND_START_LOW,
    WAITE_LOW,
  } s_State = START;

  static uint32_t up_flow;
  switch (s_State) {
    case START:
      s_State = SEND_START_LOW;

    case SEND_START_LOW:
      LOGE("send start");
      send_lower(kSynClock, 4);

      low_state = false;
      up_flow   = SEND_START_LOW;
      s_State   = WAITE_LOW;
      break;
    case WAITE_LOW:
      // LOGE("wAIT");
      if (true == low_state) {
        switch (up_flow) {
          case SEND_START_LOW:
            s_State = START;
            return fsm_cpl;
        }
      } else
        s_State = WAITE_LOW;
      break;
  }

  return fsm_onging;
}

FSM_STATE_t init_low(void) {
  /********流程********/
  /*
  1.
  2.
  3. 接收下位机初始化数据
  4.
  5.
  */

  static enum {
    START,
    RECV_INIT,
  } s_State = START;
  switch (s_State) {
    case START:
      s_State = RECV_INIT;
    case RECV_INIT:
      memcpy(up_recv_buf, low_recv_buf, low_recv_len);
      up_recv_len = low_recv_len;
      for (int i = 0; i < up_recv_len; i++)  //
        LOGI("%02x", up_recv_buf[i]);
      s_State = START;
      return fsm_cpl;
      break;
  }

  return fsm_onging;
}

FSM_STATE_t move_action_low(void) {
  /****************流程***********************/
  /*
  1. 接收下位机时钟
  2. 接收下位机数据

  */
  static enum {
    START,
    // RECV_SYN_CLOCK,
    RECV_START,
    WAITE,
  } s_State = START;

  switch (s_State) {
    case START:
      s_State = RECV_START;
    case RECV_START:
      LOGI("RECV START");
      memcpy(up_recv_buf, low_recv_buf, low_recv_len);
      up_recv_len = low_recv_len;
      for (int i = 0; i < up_recv_len; i++)  //
        LOGI("%02x", up_recv_buf[i]);
      s_State = START;
      return fsm_cpl;

      break;
  }

  return fsm_onging;
}

FSM_STATE_t inquire_low(void) {
  /****************流程***********************/
  /*
  1. 接收下位机时钟
  2. 接收下位机数据

  */
  static enum {
    START,
    // RECV_SYN_CLOCK,
    RECV_START,
    WAITE,
  } s_State = START;

  switch (s_State) {
    case START:
      s_State = RECV_START;

    case RECV_START:
      LOGI("RECV START");
      memcpy(up_recv_buf, low_recv_buf, low_recv_len);
      up_recv_len = low_recv_len;
      for (int i = 0; i < up_recv_len; i++)  //
        LOGI("%02x", up_recv_buf[i]);
      s_State = START;
      return fsm_cpl;

      break;
  }

  return fsm_onging;
}

FSM_STATE_t book_info_low(void) {
  /****************流程***********************/
  /*
  1. 接收下位机时钟
  2. 接收下位机数据

  */
  static enum {
    START,
    // RECV_SYN_CLOCK,
    RECV_START,
    WAITE,
  } s_State = START;

  switch (s_State) {
    case START:
      s_State = RECV_START;
    case RECV_START:
      LOGI("RECV START");
      memcpy(up_recv_buf, low_recv_buf, low_recv_len);
      up_recv_len = low_recv_len;
      for (int i = 0; i < up_recv_len; i++)  //
        LOGI("%02x", up_recv_buf[i]);
      s_State = START;
      return fsm_cpl;

      break;
  }

  return fsm_onging;
}

FSM_STATE_t book_num_low(void) {
  /****************流程***********************/
  /*
  1. 接收下位机时钟
  2. 接收下位机数据

  */
  static enum {
    START,
    // RECV_SYN_CLOCK,
    RECV_START,
    WAITE,
  } s_State = START;

  switch (s_State) {
    case START:
      s_State = RECV_START;
    case RECV_START:
      LOGI("RECV START");
      memcpy(up_recv_buf, low_recv_buf, low_recv_len);
      up_recv_len = low_recv_len;
      for (int i = 0; i < up_recv_len; i++)  //
        LOGI("%02x", up_recv_buf[i]);
      s_State = START;
      return fsm_cpl;

      break;
  }

  return fsm_onging;
}

FSM_STATE_t clock_low(void) {
  /****************流程***********************/
  /*
  1. 接收下位机时钟

  */
  static enum {
    START,
    // RECV_SYN_CLOCK,
    RECV_START,
    WAITE,
  } s_State = START;

  switch (s_State) {
    case START:
      s_State = RECV_START;

    case RECV_START:
      LOGI("RECV START");
      memcpy(up_recv_buf, low_recv_buf, low_recv_len);
      up_recv_len = low_recv_len;
      for (int i = 0; i < up_recv_len; i++)  //
        LOGI("%02x", up_recv_buf[i]);
      s_State = START;
      return fsm_cpl;

      break;
  }

  return fsm_onging;
}

// 先一条龙服务。上位机 -> 中 -> 下位机
void transfer_process(void) {
  CMD_HEAD_t* p_head_recv = (CMD_HEAD_t*)recv_buf;

  if (0x0B == p_head_recv->addr) {  // 下位机
    LOGI("RECV lower");
    memcpy(low_recv_buf, recv_buf, recv_len);
    low_recv_len = recv_len;
    low_start    = true;
  } else if (0x0A == p_head_recv->addr) {  // 上位机
    LOGI("RECV upper");
    memcpy(up_recv_buf, recv_buf, recv_len);
    up_recv_len = recv_len;
    upper_start = true;

    // for (int i = 0; i < up_recv_len; i++)  //
    //   LOGI("%02x", up_recv_buf[i]);
  }

  if (upper_start) {
    upper_process(up_recv_buf, up_recv_len);
  }
  if (low_start) {
    lower_process(low_recv_buf, low_recv_len);
  }

  memset(recv_buf, 0, recv_len);
  recv_len = 0;
  transmit_using(false);
}

static uint8_t revc_cmd;

void upper_process(uint8_t* packet, uint8_t len) {
  CMD_HEAD_t* p_head_recv = (CMD_HEAD_t*)packet;
  Motor_t*    servo_set   = get_servo_set_info();
  // for (int i = 0; i < data_size; i++) {
  //   LOGI("%02x", data_region[i]);
  // }
  Move_Action_t* p_action = (Move_Action_t*)(&(packet[sizeof(CMD_HEAD_t)]));

  switch (p_head_recv->cmd) {
    case kInit:

      if (!init_upper(p_head_recv->len)) {
        return;
      }

      LOGI("Init CPL");
      upper_start = false;
      break;
    case kMoveAction:
      if (!revc_cmd) {
        revc_cmd       = 1;
        servo_set->pos = p_action->x_pos;
        action_mode    = p_action->action_state;
        LOGI("ser_pos is: %04x", servo_set->pos);
        LOGI("p_act is: %04x", p_action->x_pos);
        for (int i = 0; i < sizeof(Move_Action_t); i++) {
          LOGI("pos is: %02x", packet[i]);
        }
        LOGI("cmd_head is: %x", *p_head_recv);
      }

      if (!move_action_upper(p_head_recv->len)) {
        return;
      }
      revc_cmd = 0;
      LOGI("MOVE ACTION CPL");
      upper_start = false;
      break;
    case kInquire:
      if (!inquire_upper(p_head_recv->len)) {
        return;
      }
      LOGI("INQUIRE CPL");
      upper_start = false;
      break;
    case kInquireBookNum:
      if (!book_num_upper()) {
        return;
      }
      LOGI("BOOK NUM CPL");
      upper_start = false;
      break;
    case kInquireBookInfo:
      if (!book_info_upper()) {
        return;
      }
      LOGI("BOOK INFO CPL");
      upper_start = false;
      break;

    case kSynClock:

      if (!syn_clock_upper()) {
        return;
      }
      LOGI("syn_Clock CPL");
      upper_start = false;
      break;
  }
}
void lower_process(uint8_t* pack_data, uint8_t len) {
  CMD_HEAD_t* p_head_recv = (CMD_HEAD_t*)pack_data;
  uint8_t*    data_region = (pack_data + sizeof(CMD_HEAD_t));
  uint8_t     data_size   = len - sizeof(CMD_HEAD_t) - 2 - 1;  // 1 = 帧尾，1 = size data region，2 = crc16
  // for (int i = 0; i < len; i++) {
  //   LOGI("%02x", pack_data[i]);
  // }

  switch (p_head_recv->cmd) {
    case kInit:

      if (init_low()) {
        low_start = false;
        low_state = true;
      }

      break;
    case kMoveAction:
      if (move_action_low()) {
        low_start = false;
        low_state = true;
      }

      break;
    case kInquireBookNum:
      if (book_num_low()) {
        low_start = false;
        low_state = true;
      }
      break;
    case kInquireBookInfo:
      if (book_info_low()) {
        low_start = false;
        low_state = true;
      }
      break;

    case kInquire:
      if (inquire_low()) {
        // LOGE("LOW ACTION TURE");

        low_start = false;
        low_state = true;
      }
      break;
    case kSynClock:

      if (data_size != 4) {
        LOGE("low recv clock error %d", data_size);
        break;
      }
      LOGI("RECV CLOCK");
      low_clock = (uint32_t*)data_region;
      low_clock /= 1000;
      for (int i = 0; i < 4; i++)  //
        LOGI("%02x", data_region[i]);
      low_start = false;
      low_state = true;
      break;
  }

  memset(low_recv_buf, 0, low_recv_len);
}

uint8_t* get_cmd_cache(void) { return recv_buf; }
void     set_cmd_recv_len(uint8_t len) { recv_len = len; }
void     transmit_using(bool state) { user_power = state; }
bool     get_transmit_state(void) { return user_power; }

uint8_t get_action_mode(void) { return action_mode; }