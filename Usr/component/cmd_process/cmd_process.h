#pragma
#include <stdbool.h>
#include <stdint.h>

#define SEND_BUF_LEN     256
#define DATA_REGION_SIZE 128

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

typedef enum {
  kInit            = 0x00,
  kInquireBookNum  = 0x10,
  kInquireBookInfo = 0x11,
  kMoveAction      = 0x20,
  kInquire         = 0x21,
  kSynClock        = 0x30,
} ENUM_CMD_UPPER_t;

typedef enum {
  kUpper,
  kLower,
} ENUM_CMD__t;

extern bool check_start;

void     send_lower(ENUM_CMD_UPPER_t cmd, uint8_t tail_len);
uint8_t* get_cmd_cache(void);
void     set_cmd_recv_len(uint8_t len);

void transmit_using(bool state);
bool get_transmit_state(void);
void transfer_process(void);

uint8_t get_action_mode(void);