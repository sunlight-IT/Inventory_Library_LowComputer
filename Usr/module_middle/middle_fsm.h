#pragma once

typedef enum {
  fsm_error  = -1,
  fsm_onging = 0,
  fsm_cpl    = 1,

  fsm_timeout = 2,
} FSM_STATE_t;