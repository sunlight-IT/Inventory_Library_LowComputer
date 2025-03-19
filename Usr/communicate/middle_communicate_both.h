#pragma once

#include <stdbool.h>
#include <stdint.h>

#define SEND_BUF_LEN     256
#define DATA_REGION_SIZE 128

#define WAIT_RECV_TIMEOUT 10000
#define QUEUE_NUM         16

typedef enum {
  kInit            = 0x00,
  kInquireBookNum  = 0x10,
  kInquireBookInfo = 0x11,
  kMoveAction      = 0x20,
  kInquire         = 0x21,
  kSynClock        = 0x30,
  kResetPos        = 0x40,
} ENUM_CMD_UPPER_t;

#pragma pack(1)
typedef struct {
  uint8_t head;
  uint8_t cmd;
  uint8_t encord;
  uint8_t addr;
  uint8_t len;
} CMD_HEAD_IMPROVE_t;
#pragma pack()

#pragma pack(1)
typedef struct {
  uint8_t head;
  uint8_t cmd;
  uint8_t encord;
  uint8_t addr;
  uint8_t len;
  uint8_t empty;
} CMD_HEAD_t;
#pragma pack()

typedef struct __attribute__((packed)) {
  uint8_t  empty;
  uint8_t  radar;
  uint32_t index;
  uint32_t UID_H;
  uint32_t UID_L;
  uint8_t  RSSID;
  uint32_t x_time;
  uint16_t y_pox;
  uint8_t  reak_freq;
  uint8_t  average_rssi;
} Book_Info_Lower_t;

typedef struct __attribute__((packed)) {
  uint8_t  empty;
  uint8_t  radar;
  uint32_t index;
  uint32_t UID_H;
  uint32_t UID_L;
  uint8_t  RSSID;
  uint16_t x_pos;
  uint16_t y_pox;
  uint8_t  reak_freq;
  uint8_t  average_rssi;
} Book_Info_Upper_t;

typedef struct __attribute__((packed)) {
  uint8_t  empty;
  uint16_t x_pos;
  uint16_t x_speed;
  uint16_t y_pos;
  uint16_t y_speed;
  uint8_t  action_state;
  uint8_t  direct;
  uint8_t  motor_state;
} Move_Action_t;

typedef struct __attribute__((packed)) {
  uint8_t empty;
  uint8_t reset_state;
} Reset_Mode_t;

#pragma pack(1)
typedef struct {
  uint8_t  empty;
  uint32_t index_book;
} Book_Info_Ack_Low_t;
#pragma pack()

typedef enum {
  kEmpty,
  kSend = 0x01,
  // kAckSuccess,
  kRecvSucess,
  kWationTimeOut,
  kError,
  kBookEmpty,
  kSendAck,
} LOWER_NOTION;

typedef struct __attribute__((packed)) {
  LOWER_NOTION state;
  uint8_t      cmd_head;
  uint8_t      len;
  uint8_t      data_regeion[128];
} Notion_t;

// typedef struct __attribute__((packed)) {
//   uint8_t  index[3];
//   uint8_t  UID[8];
//   uint8_t  RSSID;
//   uint32_t x_time;
//   uint16_t y_pox;
//   uint8_t  reak_freq;
//   uint8_t  average_rssi;
// } Book_Info_Lower_t;