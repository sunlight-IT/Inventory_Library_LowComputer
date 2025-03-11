#include "middle_event_process.h"

#include <stdint.h>
#include <string.h>

#include "log/my_log.h"

static EVENT_t eventQueue[EVENT_QUEUE_SIZE];
static uint8_t eventQueueHead = 0;
static uint8_t eventQueueTail = 0;

static EVENT_t m_cbs[EVENT_QUEUE_SIZE];  // 事件队列表

void registCallback(EVENT_TYPE type, event_callback cb) {
  m_cbs[type].type     = type;
  m_cbs[type].callback = cb;
}

void enterQueueEvent(EVENT_TYPE type) {
  if ((eventQueueHead + 1) % EVENT_QUEUE_SIZE == eventQueueTail) {
    LOGW("event queue full");
    return;
  }
  eventQueue[eventQueueHead].type     = type;
  eventQueue[eventQueueHead].callback = m_cbs[type].callback;
  eventQueueHead                      = (eventQueueHead + 1) % EVENT_QUEUE_SIZE;
}

void process_event(void) {
  while (eventQueueTail != eventQueueHead) {
    EVENT_t event = eventQueue[eventQueueTail];
    if (event.callback != NULL) {
      event.callback();  // 调用回调函数
    }
    eventQueueTail = (eventQueueTail + 1) % EVENT_QUEUE_SIZE;
  }
}
