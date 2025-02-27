#pragma once

#define EVENT_NUM 16

typedef void (*event_process_func)(void);

typedef enum
{
    kPackIdle,
    kWireLess,
    kCmd,
    // 预留事件处理函数
} ENUM_DATA_PACK_EVENT;

typedef struct
{
    ENUM_DATA_PACK_EVENT type;
    event_process_func func;
    // 预留事件处理函数
} Event_t;

void reg_event(Event_t *h_event);
void event_datapack_process(void);