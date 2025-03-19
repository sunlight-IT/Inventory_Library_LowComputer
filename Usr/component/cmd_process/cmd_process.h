// #pragma
// #include <stdbool.h>
// #include <stdint.h>

// #define SEND_BUF_LEN     256
// #define DATA_REGION_SIZE 128

// #define WAIT_RECV_TIMEOUT 5000
// #define QUEUE_NUM         16

// #pragma pack(1)
// typedef struct {
//   uint8_t head;
//   uint8_t cmd;
//   uint8_t encord;
//   uint8_t addr;
//   uint8_t len;
//   uint8_t empty;
// } CMD_HEAD_t;
// #pragma pack()

// #pragma pack(1)
// typedef struct {
//   uint8_t head;
//   uint8_t cmd;
//   uint8_t encord;
//   uint8_t addr;
//   uint8_t len;
// } CMD_HEAD_IMPROVE_t;
// #pragma pack()

// typedef enum {
//   kInit            = 0x00,
//   kInquireBookNum  = 0x10,
//   kInquireBookInfo = 0x11,
//   kMoveAction      = 0x20,
//   kInquire         = 0x21,
//   kSynClock        = 0x30,
// } ENUM_CMD_UPPER_t;

// // typedef enum {
// //   kUpper,
// //   kLower,
// // } ENUM_CMD__t;

// typedef struct {
//   LOWER_NOTION state;
//   uint8_t      cmd;
//   uint8_t      len;
// } Notion_t;

// typedef enum {
//   kEmpty,
//   kSend = 0x01,
//   // kAckSuccess,
//   kRecvSucess,
//   kWationTimeOut,
//   kError,
// } LOWER_NOTION;

// extern bool check_start;

// void     send_lower(ENUM_CMD_UPPER_t cmd, uint8_t tail_len);
// uint8_t* get_cmd_cache(void);
// void     set_cmd_recv_len(uint8_t len);

// void transmit_using(bool state);
// bool get_transmit_state(void);

// uint8_t get_action_mode(void);

// uint8_t* get_upper_cache(void);
// uint8_t* get_lower_cache(void);
// void     set_upper_recv_len(uint8_t len);
// void     set_lower_recv_len(uint8_t len);

// void set_upper_process(bool state);
// void set_lower_process(bool state);

// void correspondence_recv(void);
// void correspondence_process(void);