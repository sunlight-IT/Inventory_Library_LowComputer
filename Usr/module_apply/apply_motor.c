#include "apply_motor.h"
#include "module_driver/driver_wireless.h"
#include "log/my_log.h"
void cmd_process(void)
{
    uint8_t cmd_type = GetCmdType();
    switch (cmd_type)
    {
    case kIdle:
        break;
    case kStart:
        // MOTOR START
        break;
    case kStop:
        // MOTOR STOP
        break;
    default:
        LOGW("CMD NOT EXIST");
        break;
    }
}