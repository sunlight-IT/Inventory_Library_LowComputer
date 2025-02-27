#include "driver_motor.h"
#include "module_middle/middle_fsm.h"
#include "ServoMotor.h"

void motor_move(void)
{
    Uart_Send_MovementRegister_ServoMotor(3, 1); // 设置速度1，
}

FSM_STATE_t motor_move(void)
{
    static enum {
        START = 0,
        SET_SPEED,
        SET_MOVE,
    } s_State =
        {
            START};

    switch (s_State)
    {
    case START:
        s_State = SET_SPEED;
    case SET_SPEED:
    case SET_MOVE:
    }
}
FSM_STATE_t moter_send(void)
{
    static enum {
        START = 0,
        SEND_SPEED_CMD,
        IS_SUCESS,
    } s_State =
        {
            START};

    switch (s_State)
    {
    case START:
        s_State = SEND_SPEED_CMD;
    case SEND_SPEED_CMD:
    case IS_SUCESS:
    }
}