#pragma once

typedef enum {
  kRestMode = 0x40,
  kMoveMode = 0x20,
} MOTION_MODE;

// typedef struct{

// }

void app_servo_action_init(void);
void app_action(void);