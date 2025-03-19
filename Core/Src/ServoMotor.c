/*
 * ServoMotor.c
 *
 *  Created on: Nov 1, 2024
 *      Author: 45951
 */

#include "ServoMotor.h"

#include "log/my_log.h"
#include "module_driver/driver_motor.h"
#include "module_driver/driver_wireless.h"
#include "module_middle/middle_event_process.h"

struct S_Motor {
  uint8_t  IsPowerOn;         // 电机电源
  uint8_t  IsConnected;       // 串口连接
  uint8_t  IsReset;           // 电机复位
  double   EndPosition;       // 运动末尾
  double   ChargingPosition;  // 充电位置
  double   distance;          ////电机移动距离
  int32_t  pulses;            // 电机脉冲
  int8_t   direction;         // 电机运动方向
  int8_t   MotorPosition;     // 电机位置，1右，-1左
  uint16_t DutyRatio;         // PWM占空比
  int16_t  speed;             // 电机速度
  uint8_t  address;
  uint16_t UartCurrentCmd[3];  // UartCurrentCmd[0]:读写功能（0x00，结束），UartCurrentCmd[1]:目标地址，UartCurrentCmd[2]:word数量
  uint16_t TimerFlag[4];
  uint8_t  Flag_Event[4];
};
struct S_Motor ServoMotor;
uint8_t        Set_null_ServoMotor[1]            = {0};
uint8_t        Rxdbuf_ServoMotor[1]              = {0};
uint8_t        CntRxd_ServoMotor                 = 0;
uint8_t        UartRxd_ServoMotor[255]           = {0};
uint8_t        Flag_UartRxdMonitor_ServoMotor    = 0;
uint8_t        UartSend_ServoMotor[255]          = {0};
uint8_t        Flag_UartRegroup_ASCII_ServoMotor = 0;

/****************自己的************************/

static uint8_t motor_ack[2];
static uint8_t motor_state_reg[4];

static UART_HandleTypeDef *m_uart;
static DMA_HandleTypeDef  *m_dma;

static void moteruart_receive(void);

void motor_uart_init(void) {  //
  registCallback(EVENT_MotorUart, moteruart_receive);
}

void motor_reg_handle(UART_HandleTypeDef *h_uart, DMA_HandleTypeDef *h_dma) {
  m_uart = h_uart;
  m_dma  = h_dma;
}

void motor_init(void) {
  registCallback(EVENT_MotorUart, moteruart_receive);  // 注册事件表
  // HAL_UARTEx_ReceiveToIdle_DMA(m_uart, rx_buf, sizeof(rx_buf));  // 串口�?�?+dma
  // __HAL_DMA_DISABLE_IT(m_dma, DMA_IT_HT);                        // 关闭dma接收半满�?�?函数
}

uint8_t *GetMoterAck(void) { return motor_ack; }
uint8_t *GetMoterStateReg(void) { return motor_state_reg; }
int8_t   GetMoterOnPos(void) { return ServoMotor.MotorPosition; }
uint8_t  GetMoterFlag(void) { return ServoMotor.Flag_Event[2]; }
/*****************************************/

void test(uint8_t *buf) {
  for (uint8_t i = 0; i < 4; i++) {
    buf[i + 1] = ServoMotor.Flag_Event[i];
  }
}
void Events_ServoMotor(void) {
  UartDriver_ServoMotor();
  switch (ServoMotor.Flag_Event[0]) {
    case 0: {
      Event_Init_ServoMotor();
    } break;
    case 1: {
      Event_Movement_ServoMotor();
    } break;
  }
}

void Event_Init_ServoMotor(void) {
  // LOGI("STATE : %d", ServoMotor.Flag_Event[1]);
  switch (ServoMotor.Flag_Event[1]) {
    case 0: {
      SetStatus_Power_ServoMotor(PowerON_ServoMotor);  // 打开伺服驱动器的电源
      UartReceiveIT_ServoMotor();                      // 打开伺服驱动器的串口接收
      ServoMotor.address       = 0x01;                 // 伺服电机地址设置
      ServoMotor.MotorPosition = ON_LEFT;
      ServoMotor.Flag_Event[1] = 1;  // 定时器定时变2，因为伺服驱动器启动时间大约5s
    } break;
    case 2: {
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[0]);
      UartSend_MonitorRegister_ServoMotor(0);  // 串口发送：检测状态监控寄存器1-5设置指令,发送是Read_Func，10words
      ServoMotor.Flag_Event[1] = 3;            // 等待串口返回，返回正确赋值14，错误赋值4，串口连上则赋值IsConnected = 1，超时5s：定时器赋值2重新发送
    } break;
    case 4: {  // 状态监控寄存器1-5配置错误，发送状态监控寄存器1配置
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[0]);
      UartSend_MonitorRegister_ServoMotor(1);  // 设置状态监控寄存器1，发送是Write_Once_Func
      ServoMotor.Flag_Event[1] = 5;            // 等待串口返回，返回正确赋值6，错误或者超时2s，赋值4
    } break;
    case 6: {  // 状态监控寄存器2配置
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[0]);
      UartSend_MonitorRegister_ServoMotor(2);  // 设置状态监控寄存器2，发送是Write_Once_Func
      ServoMotor.Flag_Event[1] = 7;            // 等待串口返回，返回正确赋值8，错误或者超时2s，赋值6
    } break;
    case 8: {  // 状态监控寄存器3配置
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[0]);
      UartSend_MonitorRegister_ServoMotor(3);  // 设置状态监控寄存器3，发送是Write_Once_Func
      ServoMotor.Flag_Event[1] = 9;            // 等待串口返回，返回正确赋值10，错误或者超时2s，赋值8
    } break;
    case 10: {  // 状态监控寄存器4配置
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[0]);
      UartSend_MonitorRegister_ServoMotor(4);  // 设置状态监控寄存器4，发送是Write_Once_Func
      ServoMotor.Flag_Event[1] = 11;           // 等待串口返回，返回正确赋值12，错误或者超时2s，赋值10
    } break;
    case 12: {  // 状态监控寄存器5配置
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[0]);
      UartSend_MonitorRegister_ServoMotor(5);  // 设置状态监控寄存器5，发送是Write_Once_Func
      ServoMotor.Flag_Event[1] = 13;           // 等待串口返回，返回正确赋值2,重读寄存器的状态，错误或者超时2s，赋值12
    } break;
    case 14: {  // 状态监控寄存器都是正确的
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[0]);
      UartSend_MonitorRegister_ServoMotor(6);  // 查询电子齿轮比分子分母的设置,发送是Read_Func，4words
      ServoMotor.Flag_Event[1] = 15;           // 等待串口返回，返回正确赋值18,重读寄存器的设置，错误赋值16，超时2s，赋值14
    } break;
    case 16: {
      //
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[0]);
      UartSend_MonitorRegister_ServoMotor(7);  // 电子齿轮比分子分母的设置，发送是Write_Multi_Func，4words
      ServoMotor.Flag_Event[1] = 17;           // 等待串口返回，返回正确赋值14,重读寄存器的设置，错误或者超时2s，赋值16
    } break;
    case 18: {  // 读取脉冲清除模式，需要设置为0x11;
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[0]);
      UartSend_MonitorRegister_ServoMotor(8);  // 读取脉冲清除模式，需要设置为0x11，发送是Read_Func，1words
      ServoMotor.Flag_Event[1] = 19;           // 等待串口返回，返回正确赋值22,重读寄存器的设置，错误赋值20，超时2s，赋值18
    } break;
    case 20: {  // 脉冲清除模式，需要设置为0x11;
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[0]);
      UartSend_MonitorRegister_ServoMotor(9);  // 脉冲清除模式，需要设置为0x11，发送是Write_Once_Func，1words
      ServoMotor.Flag_Event[1] = 21;           // 等待串口返回，返回正确赋值18,重读寄存器的设置，错误超时2s，赋值20
    } break;
    case 22: {  //
      // 脉冲模式设置正确，读取EEPROM是否为不保存模式
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[0]);
      UartSend_MonitorRegister_ServoMotor(10);  // 读取EEPROM是否为不保存模式，发送是Read_Func，1words
      ServoMotor.Flag_Event[1] = 23;            // 等待串口返回，返回正确赋值26,重读寄存器的设置，错误赋值24，超时2s，赋值22
    } break;
    case 24: {
      //
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[0]);
      UartSend_MonitorRegister_ServoMotor(11);  // 设置EEPROM为不保存模式，发送是Write_Once_Func，1words
      ServoMotor.Flag_Event[1] = 25;            // 等待串口返回，返回正确赋值22,重读寄存器的设置，错误超时2s，赋值24
    } break;
    case 26: {  // 设置了EEPROM不保存模式
      ServoMotor.Flag_Event[0] = 1;
    } break;
  }
}
void UartAction_ServoMotor(uint8_t *buf, uint8_t len) {
  uint8_t uartbuf[255] = {0};
  uint8_t lenbuf       = len / 2;
  for (uint8_t i = 0; i < lenbuf; i++) {
    uartbuf[i] = Get_Regroup_ASCII(buf[2 * i], 0) | Get_Regroup_ASCII(buf[2 * i + 1], 1);
  }

  if (uartbuf[0] != ServoMotor.address && uartbuf[1] != (uint8_t)(ServoMotor.UartCurrentCmd[0] & 0xFF)) {
    return;
  }
  UartWrite_WirelessModule(uartbuf, lenbuf);
  switch (uartbuf[1]) {
    case Cmd_Read_Func_ServoMotor: {
      switch (ServoMotor.UartCurrentCmd[1]) {  // 寄存器
        case MonitorRegister_1_Data: {         // 含电机脉冲、扭矩、速度等
        } break;
        case MonitorRegister_1_Set: {  // 状态寄存器1设置地址，需要设置该位读取“回授位置（PUU）自定义单位，00h”
          switch (ServoMotor.UartCurrentCmd[2]) {
            case 1: {  // 单字节查询
            } break;
            case 10: {  // 查询状态监控寄存器1-5
              ServoMotor.IsConnected         = 1;
              uint8_t MonitorRegisterSet_buf = 0;
              // 检查uartbuf[4]的值，如果不等于PulseMonitor_0022H的低8位，则设置MonitorRegisterSet_buf的第0位（0x01）
              if (uartbuf[4] != (PulseMonitor_0022H & 0xFF)) {
                MonitorRegisterSet_buf |= 0x01;
              }
              // 检查uartbuf[8]的值，如果不等于TorqueMonitor_0024H的低8位，则设置MonitorRegisterSet_buf的第1位（0x02）
              if (uartbuf[8] != (TorqueMonitor_0024H & 0xFF)) {
                MonitorRegisterSet_buf |= 0x02;
              }
              // 检查uartbuf[12]的值，如果不等于SpeedMonitor_0026H的低8位，则设置MonitorRegisterSet_buf的第2位（0x04）
              if (uartbuf[12] != (SpeedMonitor_0026H & 0xFF)) {
                MonitorRegisterSet_buf |= 0x04;
              }
              // 检查uartbuf[16]的值，如果不等于Monitor_0028H的低8位，则设置MonitorRegisterSet_buf的第3位（0x08）
              if (uartbuf[16] != (Monitor_0028H & 0xFF)) {
                MonitorRegisterSet_buf |= 0x08;
              }
              // 检查uartbuf[20]的值，如果不等于Monitor_002AH的低8位，则设置MonitorRegisterSet_buf的第4位（0x10）
              if (uartbuf[20] != (Monitor_002AH & 0xFF)) {
                MonitorRegisterSet_buf |= 0x10;
              }
              if (MonitorRegisterSet_buf) {    // 有配置错误
                ServoMotor.Flag_Event[1] = 4;  // 重新设置状态监控寄存器1
              } else {                         // 配置正确
                ServoMotor.Flag_Event[1] = 14;
              }
            } break;
          }
        } break;
        case GearRatioRegister_Numerator: {  // 电子齿轮比分子分母寄存器，需要设置分子160，分母94
          switch (ServoMotor.UartCurrentCmd[2]) {
            case 4: {
              uint8_t GearRatioRegisterSet_buf = 0;
              // 检查uartbuf[4]的值，如果不等于PulseMonitor_0022H的低8位，则设置MonitorRegisterSet_buf的第0位（0x01）
              if (uartbuf[4] != (GearRatio_Numerator_0158H & 0xFF)) {
                GearRatioRegisterSet_buf |= 0x01;
              }
              // 检查uartbuf[8]的值，如果不等于TorqueMonitor_0024H的低8位，则设置MonitorRegisterSet_buf的第1位（0x02）
              if (uartbuf[8] != (GearRatio_Denominator_015AH & 0xFF)) {
                GearRatioRegisterSet_buf |= 0x02;
              }
              if (GearRatioRegisterSet_buf) {   // 有配置错误
                ServoMotor.Flag_Event[1] = 16;  // 重新设置电子齿轮比分子分母寄存器
              } else {                          // 配置正确
                ServoMotor.Flag_Event[1] = 18;
              }
            } break;
          }
        } break;
        case AuxrFuncRegister_P2_30: {  // 0:关闭所有下述功能;1：强制软件 Servo On。5：设定可防止连续写入EEPROM,若使用通讯控制时必需将此参数设定。
          switch (ServoMotor.UartCurrentCmd[2]) {
            case 1: {
              if (uartbuf[4] == (EEPROM_Disable_P2_30 & 0xFF)) {  // 已经设置了EEPROM不保存
                ServoMotor.Flag_Event[1] = 26;
              } else {
                ServoMotor.Flag_Event[1] = 24;
              }
            } break;
          }
        } break;
        case PulseReset_Register_P2_50: {  // 脉冲清除模式，需要设置为0x11;
          switch (ServoMotor.UartCurrentCmd[2]) {
            case 1: {
              if (uartbuf[4] == (PulseReset_Data_P2_50 & 0xFF)) {  // 已经设置脉冲清除模式，为0x11;
                ServoMotor.Flag_Event[1] = 22;
              } else {
                ServoMotor.Flag_Event[1] = 20;
              }
            } break;
          }
        } break;
      }
    } break;
    case Cmd_Write_Once_Func_ServoMotor: {
      switch (ServoMotor.UartCurrentCmd[1]) {  // 读取寄存器
        case MonitorRegister_1_Set: {          // 状态监控寄存器1的设置
          ServoMotor.Flag_Event[1] = 6;        // 设置状态监控寄存器2
        } break;
        case MonitorRegister_2_Set: {    // 状态监控寄存器2的设置
          ServoMotor.Flag_Event[1] = 8;  // 设置状态监控寄存器3
        } break;
        case MonitorRegister_3_Set: {     // 状态监控寄存器3的设置
          ServoMotor.Flag_Event[1] = 10;  // 设置状态监控寄存器4
        } break;
        case MonitorRegister_4_Set: {     // 状态监控寄存器4的设置
          ServoMotor.Flag_Event[1] = 12;  // 设置状态监控寄存器5
        } break;
        case MonitorRegister_5_Set: {    // 状态监控寄存器5的设置
          ServoMotor.Flag_Event[1] = 2;  // 重新检查状态监控寄存器1-5
        } break;
        case PT_Szmode_Register_P1_01: {
          switch (ServoMotor.Flag_Event[2]) {
              // case 17:
              // { // 写入PT模式接收到了
              // 	ServoMotor.Flag_Event[2] = PTmode_Data_P1_01 == (((uint16_t)uartbuf[4] << 8) | uartbuf[5]) ? 18 : 16;
              // }
              // break;
            case 23: {  // 写入Sz模式接收到了
              ServoMotor.Flag_Event[2] = Szmode_Data_P1_01 == (((uint16_t)uartbuf[4] << 8) | uartbuf[5]) ? 24 : 22;
            } break;
          }
        } break;
        case DI1_Register_P2_10: {
          switch (ServoMotor.Flag_Event[2]) {
            case 5: {  // 伺服开启指令接收到了
              ServoMotor.Flag_Event[2] = DI1_Data_ServoON == (((uint16_t)uartbuf[4] << 8) | uartbuf[5]) ? 6 : 4;
            } break;
            case 25: {  // 伺服开启指令接收到了
              ServoMotor.Flag_Event[2] = DI1_Data_ServoON == (((uint16_t)uartbuf[4] << 8) | uartbuf[5]) ? 26 : 24;
            } break;
          }
        } break;
        case DI2_PulseClear_Register_P2_11: {
          switch (ServoMotor.Flag_Event[2]) {
            case 19: {  // DI2设置，脉冲清除
              ServoMotor.Flag_Event[2] = DI2_PulseClear_Start_Data_P2_11 == (((uint16_t)uartbuf[4] << 8) | uartbuf[5]) ? 20 : 16;
            } break;
            case 21: {  // DI2复位，恢复脉冲计数
              ServoMotor.Flag_Event[2] = DI2_PulseClear_End_Data_P2_11 == (((uint16_t)uartbuf[4] << 8) | uartbuf[5]) ? 22 : 20;
            } break;
          }
        } break;
        case AuxrFuncRegister_P2_30: {
          ServoMotor.Flag_Event[1] = 22;  // 重新读取EEPROM设置是否为不保存模式
        } break;
        case PulseReset_Register_P2_50: {
          ServoMotor.Flag_Event[1] = 18;  // 重新读取脉冲清除模式
        } break;
        case JOGmode_Register_P4_05: {
          switch (ServoMotor.Flag_Event[2]) {
            case 3: {  // 发送了速度0,先要确保停下来，不然开启伺服会冲
              ServoMotor.Flag_Event[2] = (uartbuf[4] == 0x00 && uartbuf[5] == 0x00) ? 4 : 2;
            } break;
            case 7: {  // 发送了速度5
              ServoMotor.Flag_Event[2] = (uartbuf[4] == 0x00 && uartbuf[5] == 0x14) ? 8 : 6;
            } break;
            case 9: {
              if (ServoMotor.MotorPosition == Right_Position_ServoMotor) {  // 右//CCW往左动JOGmode_Data_CCW
                ServoMotor.Flag_Event[2] = JOGmode_Data_CCW == (((uint16_t)uartbuf[4] << 8) | uartbuf[5]) ? 10 : 8;
              } else if (ServoMotor.MotorPosition == Left_Position_ServoMotor) {  // 左//CW往右动
                ServoMotor.Flag_Event[2] = JOGmode_Data_CW == (((uint16_t)uartbuf[4] << 8) | uartbuf[5]) ? 10 : 8;
              }
            } break;
            case 11: {  // 刚好离开，马上停止
              ServoMotor.Flag_Event[2] = (uartbuf[4] == 0x00 && uartbuf[5] == 0x00) ? 12 : 10;
            } break;
            case 13: {                                                      // 需要再次返回到传感器
              if (ServoMotor.MotorPosition == Right_Position_ServoMotor) {  // 右//CW往右动
                ServoMotor.Flag_Event[2] = JOGmode_Data_CW == (((uint16_t)uartbuf[4] << 8) | uartbuf[5]) ? 14 : 12;
              } else if (ServoMotor.MotorPosition == Left_Position_ServoMotor) {  // 左//CCW往左动
                ServoMotor.Flag_Event[2] = JOGmode_Data_CCW == (((uint16_t)uartbuf[4] << 8) | uartbuf[5]) ? 14 : 12;
              }
            } break;
            case 15: {  // 刚好回去
              ServoMotor.Flag_Event[2] = (uartbuf[4] == 0x00 && uartbuf[5] == 0x00) ? 16 : 14;
            } break;
            case 17: {
              ServoMotor.Flag_Event[2] = DI2_PulseClear_Start_Data_P2_11 == (((uint16_t)uartbuf[4] << 8) | uartbuf[5]) ? 18 : 16;
              // ServoMotor.Flag_Event[2] = (uartbuf[4] == 0x00 && uartbuf[5] == 0x01) ? 18 : 16;
            } break;
            case 19: {
              if (ServoMotor.MotorPosition == Right_Position_ServoMotor) {  // 右//CCW往左动JOGmode_Data_CCW
                ServoMotor.Flag_Event[2] = JOGmode_Data_CCW == (((uint16_t)uartbuf[4] << 8) | uartbuf[5]) ? 20 : 18;
              } else if (ServoMotor.MotorPosition == Left_Position_ServoMotor) {  // 左//CW往右动
                ServoMotor.Flag_Event[2] = JOGmode_Data_CW == (((uint16_t)uartbuf[4] << 8) | uartbuf[5]) ? 20 : 18;
              }
            } break;
            case 27: {  // 发送了速度1
              ServoMotor.Flag_Event[2] = (uartbuf[4] == 0x00 && uartbuf[5] == 0x00) ? 28 : 26;
            } break;
            case 29: {
              // 需要再次返回到传感器
              if (ServoMotor.MotorPosition == Right_Position_ServoMotor) {  // 右//CW往右动
                ServoMotor.Flag_Event[2] = JOGmode_Data_CW == (((uint16_t)uartbuf[4] << 8) | uartbuf[5]) ? 30 : 28;
              } else if (ServoMotor.MotorPosition == Left_Position_ServoMotor) {  // 左//CCW往左动
                ServoMotor.Flag_Event[2] = JOGmode_Data_CCW == (((uint16_t)uartbuf[4] << 8) | uartbuf[5]) ? 30 : 28;
              }
            } break;
          }
        } break;
      }
    } break;
    case Cmd_Write_Multi_Func_ServoMotor: {
      switch (ServoMotor.UartCurrentCmd[1]) {  // 读取寄存器
        case GearRatioRegister_Numerator: {    // 电子齿轮比分子分母寄存器，需要设置分子160，分母94
          switch (ServoMotor.UartCurrentCmd[2]) {
            case 4: {
              if (uartbuf[5] == ServoMotor.UartCurrentCmd[2]) ServoMotor.Flag_Event[1] = 14;  // 重新电子齿轮比分子分母寄存器
            } break;
          }
        } break;
      }
    } break;
  }
}
void Event_Movement_ServoMotor(void) {
  // LOGI("STATE : %d", ServoMotor.Flag_Event[2]);
  switch (ServoMotor.Flag_Event[2]) {
    case 0: {                     // 判断是否校准
      if (!ServoMotor.IsReset) {  // 没有校准
        ServoMotor.Flag_Event[2] = 1;
      }
    } break;
    case 1: {                                                                                                            // 没有校准的时候
      if (LimitSensorON_ServoMotor == HAL_GPIO_ReadPin(LimitSensor_ServoMotor_GPIO_Port, LimitSensor_ServoMotor_Pin)) {  // 一开始就有传感器触发的情况
        ServoMotor.Flag_Event[2] = 2;
      } else {
        ServoMotor.Flag_Event[2] = 2;
      }
    } break;
    case 2: {  // 设备最低速度缓慢运动然后当刚离开传感器范围，随后又返回至刚触发瞬间
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[1]);
      Uart_Send_MovementRegister_ServoMotor(3, 0);  // 设置速度0，如果一开始不设置速度0的话，可能会冲出去
      ServoMotor.Flag_Event[2] = 3;                 // 等待串口返回，赋值4，超时赋值2
    } break;
    case 4: {
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[1]);
      Uart_Send_MovementRegister_ServoMotor(1, DI1_Data_ServoON);  // 打开伺服控制Servo ON
      ServoMotor.Flag_Event[2] = 5;                                // 等待串口返回，赋值6，超时赋值4
    } break;
    case 6: {
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[1]);
      // Uart_Send_MovementRegister_ServoMotor(3, 20);  // 设置速度1

      ServoMotor.Flag_Event[2] = 16;  // 等待串口返回，赋值8，超时赋值6
    } break;
    case 8: {
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[1]);
      // 判断电机方向 ServoMotor.MotorPosition，在右则往左动，在左则往右动
      if (ServoMotor.MotorPosition == Right_Position_ServoMotor) {        // 右
        Uart_Send_MovementRegister_ServoMotor(3, JOGmode_Data_CCW);       // CCW往左动JOGmode_Data_CCW
      } else if (ServoMotor.MotorPosition == Left_Position_ServoMotor) {  // 左
        Uart_Send_MovementRegister_ServoMotor(3, JOGmode_Data_CW);        // CW往右动
      }
      ServoMotor.Flag_Event[2] = 9;  // 等待串口返回，赋值10，超时赋值8
    } break;
    case 10: {
      if (LimitSensorOFF_ServoMotor == HAL_GPIO_ReadPin(LimitSensor_ServoMotor_GPIO_Port, LimitSensor_ServoMotor_Pin)) {  // 刚好离开，马上停止
        ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[1]);
        Uart_Send_MovementRegister_ServoMotor(3, 0);  // 设置速度0，停止
        ServoMotor.Flag_Event[2] = 11;                // 等待串口返回，赋值12，超时赋值10
      }
    } break;
    case 12: {
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[1]);
      // 判断电机方向 ServoMotor.MotorPosition，在右则往右动，在左则往左动
      if (ServoMotor.MotorPosition == Right_Position_ServoMotor) {        // 右
        Uart_Send_MovementRegister_ServoMotor(3, JOGmode_Data_CW);        // CW往右动
      } else if (ServoMotor.MotorPosition == Left_Position_ServoMotor) {  // 左
        Uart_Send_MovementRegister_ServoMotor(3, JOGmode_Data_CCW);       // CCW往左动
      }
      ServoMotor.Flag_Event[2] = 13;  // 等待串口返回，赋值14，超时赋值12
    } break;
    case 14: {
      if (LimitSensorON_ServoMotor == HAL_GPIO_ReadPin(LimitSensor_ServoMotor_GPIO_Port, LimitSensor_ServoMotor_Pin)) {  // 刚好回去
        ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[1]);
        Uart_Send_MovementRegister_ServoMotor(3, 0);  // 设置速度0，停止
        ServoMotor.Flag_Event[2] = 15;                // 等待串口返回，赋值16，超时赋值14
      }
      if (0) {  // 扭矩过大要停下来
        ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[1]);
        Uart_Send_MovementRegister_ServoMotor(3, 0);
        ServoMotor.Flag_Event[2] = 0;
      }
    } break;
    // 缺一个开始信号
    case 16: {
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[1]);

      // Uart_Send_MovementRegister_ServoMotor(4, DI2_PulseClear_Start_Data_P2_11);
      ServoMotor.Flag_Event[2] = 16;  // 等待串口返回，赋值18，超时赋值16

    } break;
    case 20: {  // 设置了PT模式，发送清除脉冲的指令，设置DI2
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[1]);
      // Uart_Send_MovementRegister_ServoMotor(4, DI2_PulseClear_End_Data_P2_11);
      ServoMotor.Flag_Event[2] = 21;  // 等待串口返回，赋值20，超时赋值18
    } break;
    case 22: {  // 设置了PT模式，发送清除脉冲的指令，复位DI2
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[1]);
      ServoMotor.Flag_Event[2] = 22;
      // Uart_Send_MovementRegister_ServoMotor(4, DI2_PulseClear_End_Data_P2_11); // 已经归位，重置脉冲计数,写入PT模式
      // ServoMotor.Flag_Event[2] = 21; // 等待串口返回，赋值22，超时赋值20
    } break;
    case 23: {  // 脉冲已经重置了，设置成Sz模式
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[1]);
      // Uart_Send_MovementRegister_ServoMotor(4, Szmode_Data_P1_01); // 已经归位，重置脉冲计数,写入PT模式
      ServoMotor.Flag_Event[2] = 22;  // 等待串口返回，赋值24，超时赋值22
    } break;
    case 24: {
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[1]);
      // Uart_Send_MovementRegister_ServoMotor(1, DI1_Data_ServoON); // 打开伺服控制Servo ON
      ServoMotor.Flag_Event[2] = 25;  // 等待串口返回，赋值6，超时赋值4
    } break;
    case 26: {
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[1]);
      // Uart_Send_MovementRegister_ServoMotor(3, 1); // 设置速度1
      ServoMotor.Flag_Event[2] = 27;  // 等待串口返回，赋值26，超时赋值24
    } break;
    case 28: {
      ResetTimerFlag_ServoMotor(&ServoMotor.TimerFlag[1]);
      ServoMotor.Flag_Event[2] = 29;  // 等待串口返回，赋值28，超时赋值26
    } break;
  }
}
void TimerTasks_ServoMotor(void) {
  UartRxdMonitor_ServoMotor(1);
  if (ServoMotor.Flag_Event[1] == 1) {  // 初始化上电延迟
    ServoMotor.TimerFlag[0]++;
    if (ServoMotor.TimerFlag[0] >= 5000) {
      ServoMotor.TimerFlag[0]  = 0;
      ServoMotor.Flag_Event[1] = 2;  // 检查状态监控寄存器1-5状态
    }
  } else if (ServoMotor.Flag_Event[1] == 3 ||   //
             ServoMotor.Flag_Event[1] == 5 ||   //
             ServoMotor.Flag_Event[1] == 7 ||   //
             ServoMotor.Flag_Event[1] == 9 ||   //
             ServoMotor.Flag_Event[1] == 11 ||  //
             ServoMotor.Flag_Event[1] == 13 ||  //
             ServoMotor.Flag_Event[1] == 15 ||  //
             ServoMotor.Flag_Event[1] == 17 ||  //
             ServoMotor.Flag_Event[1] == 19 ||  //
             ServoMotor.Flag_Event[1] == 21 ||  //
             ServoMotor.Flag_Event[1] == 23 ||  //
             ServoMotor.Flag_Event[1] == 25 ||  //
             ServoMotor.Flag_Event[1] == 27) {  // 初始化检查指令，必须设置正确
    ServoMotor.TimerFlag[0]++;
    if (ServoMotor.TimerFlag[0] >= 1000) {
      ServoMotor.TimerFlag[0]  = 0;
      ServoMotor.Flag_Event[1] = ServoMotor.Flag_Event[1] - 1;  // 超时没反应，返回上一次发送
    }
  }
}
void ResetTimerFlag_ServoMotor(uint16_t *TimerFlag) { *TimerFlag = 0; }
void Uart_Send_MovementRegister_ServoMotor(uint8_t mode, uint16_t Data) {
  // LOGI("Send mov massage");
  switch (mode) {
    case 0: {  // 查询监视器1-5内容
      Uart_ReadWriteRegister_ServoMotor(Cmd_Read_Func_ServoMotor, 10, MonitorRegister_1_Data, 0, Set_null_ServoMotor);
    } break;
    case 1: {  // 开启 或 关闭伺服控制
      Uart_ReadWriteRegister_ServoMotor(Cmd_Write_Once_Func_ServoMotor, 0, DI1_Register_P2_10, Data, Set_null_ServoMotor);
    } break;
    case 2: {  // 写入PT或Sz模式,写入4，设置速度模式，写入0，设置位置模式
      Uart_ReadWriteRegister_ServoMotor(Cmd_Write_Once_Func_ServoMotor, 0, PT_Szmode_Register_P1_01, Data, Set_null_ServoMotor);
    } break;
    case 3: {  // 速度控制
      Uart_ReadWriteRegister_ServoMotor(Cmd_Write_Once_Func_ServoMotor, 0, JOGmode_Register_P4_05, Data, Set_null_ServoMotor);
    } break;
    case 4: {  // DI2设置0x0004-脉冲清除,0x0104	//脉冲恢复计数
      Uart_ReadWriteRegister_ServoMotor(Cmd_Write_Once_Func_ServoMotor, 0, DI2_PulseClear_Register_P2_11, Data, Set_null_ServoMotor);
    } break;
  }
}
void UartSend_MonitorRegister_ServoMotor(uint8_t mode) {
  switch (mode) {
    case 0: {  // 问询监视器1-5寄存器配置是否正确
      Uart_ReadWriteRegister_ServoMotor(Cmd_Read_Func_ServoMotor, 10, MonitorRegister_1_Set, 0, Set_null_ServoMotor);
    } break;
    case 1: {  // 写入正确的监视器1寄存器
      Uart_ReadWriteRegister_ServoMotor(Cmd_Write_Once_Func_ServoMotor, 0, MonitorRegister_1_Set, PulseMonitor_0022H, Set_null_ServoMotor);
    } break;
    case 2: {  // 写入正确的监视器2寄存器
      Uart_ReadWriteRegister_ServoMotor(Cmd_Write_Once_Func_ServoMotor, 0, MonitorRegister_2_Set, TorqueMonitor_0024H, Set_null_ServoMotor);
    } break;
    case 3: {  // 写入正确的监视器3寄存器
      Uart_ReadWriteRegister_ServoMotor(Cmd_Write_Once_Func_ServoMotor, 0, MonitorRegister_3_Set, SpeedMonitor_0026H, Set_null_ServoMotor);
    } break;
    case 4: {  // 写入正确的监视器4寄存器
      Uart_ReadWriteRegister_ServoMotor(Cmd_Write_Once_Func_ServoMotor, 0, MonitorRegister_4_Set, Monitor_0028H, Set_null_ServoMotor);
    } break;
    case 5: {  // 写入正确的监视器5寄存器
      Uart_ReadWriteRegister_ServoMotor(Cmd_Write_Once_Func_ServoMotor, 0, MonitorRegister_5_Set, Monitor_002AH, Set_null_ServoMotor);
    } break;
    case 6: {  // 查询电子齿轮比分子分母的设置
      Uart_ReadWriteRegister_ServoMotor(Cmd_Read_Func_ServoMotor, 4, GearRatioRegister_Numerator, 0, Set_null_ServoMotor);
    } break;
    case 7: {  // 写入正确的电子齿轮比分子分母的设置，分子160，分母94，对应的皮带94mm一圈，计算得出1000脉冲/mm
      uint8_t uartsendbuf[8] = {0x00, 0xA0, 0x00, 0x00, 0x00, 0x5E, 0x00, 0x00};
      Uart_ReadWriteRegister_ServoMotor(Cmd_Write_Multi_Func_ServoMotor, 4, GearRatioRegister_Numerator, 0, uartsendbuf);
    } break;
    case 8: {  // 读一次脉冲清除模式，需要设置为0x11;
      Uart_ReadWriteRegister_ServoMotor(Cmd_Read_Func_ServoMotor, 1, PulseReset_Register_P2_50, 0, Set_null_ServoMotor);
    } break;
    case 9: {  // 设置脉冲清除模式，需要设置为0x11;
      Uart_ReadWriteRegister_ServoMotor(Cmd_Write_Once_Func_ServoMotor, 0, PulseReset_Register_P2_50, PulseReset_Data_P2_50, Set_null_ServoMotor);
    } break;
    case 10: {  // 读一次EEPROM的设置，0:关闭所有下述功能;1：强制软件 Servo On。5：设定可防止连续写入EEPROM,若使用通讯控制时必需将此参数设定。
      Uart_ReadWriteRegister_ServoMotor(Cmd_Read_Func_ServoMotor, 1, AuxrFuncRegister_P2_30, 0, Set_null_ServoMotor);
    } break;
    case 11: {  // 设置EEPROM的为不保存模式，0:关闭所有下述功能;1：强制软件 Servo On。5：设定可防止连续写入EEPROM,若使用通讯控制时必需将此参数设定。
      Uart_ReadWriteRegister_ServoMotor(Cmd_Write_Once_Func_ServoMotor, 0, AuxrFuncRegister_P2_30, EEPROM_Disable_P2_30, Set_null_ServoMotor);
    } break;
  }
}

uint16_t Get_LRC_ServoMotor(uint8_t *buf, uint8_t len) {
  uint16_t sumbuf = 0;
  uint8_t  result = 0, result_h, result_l;
  uint16_t LRCbuf = 0;
  for (uint8_t i = 0; i < len; i++) {
    sumbuf += buf[i];
  }
  result   = ~(sumbuf & 0xFF) + 1;
  result_h = result / 0x10;
  result_l = result % 0x10;
  if (0x00 <= result_h && result_h <= 0x09) {
    LRCbuf |= (uint16_t)(result_h + '0') << 8;
  } else if (0x0A <= result_h && result_h <= 0x0F) {
    LRCbuf |= (uint16_t)(result_h - 0x0A + 'A') << 8;
  }
  if (0x00 <= result_l && result_l <= 0x09) {
    LRCbuf |= result_l + '0';
  } else if (0x0A <= result_l && result_l <= 0x0F) {
    LRCbuf |= result_l - 0x0A + 'A';
  }
  return LRCbuf;
}
void UartWrite_HexToAscii_AddLRC_ServoMotor(uint8_t *buf, uint8_t len) {
  uint8_t  Sendbuf[255] = {0};
  uint8_t  lenbuf       = 2 * len + 5;
  uint16_t LRCbuf       = Get_LRC_ServoMotor(buf, len);
  uint8_t  buf_h, buf_l;
  Sendbuf[0]          = ':';
  Sendbuf[lenbuf - 4] = LRCbuf >> 8;
  Sendbuf[lenbuf - 3] = LRCbuf & 0xFF;
  Sendbuf[lenbuf - 2] = 0x0D;
  Sendbuf[lenbuf - 1] = 0x0A;
  for (uint8_t i = 0; i < len; i++) {
    buf_h = buf[i] / 0x10;
    buf_l = buf[i] % 0x10;
    if (0x00 <= buf_h && buf_h <= 0x09) {
      Sendbuf[2 * i + 1] = buf_h + '0';
    } else if (0x0A <= buf_h && buf_h <= 0x0F) {
      Sendbuf[2 * i + 1] = buf_h - 0x0A + 'A';
    }
    if (0x00 <= buf_l && buf_l <= 0x09) {
      Sendbuf[2 * i + 2] = buf_l + '0';
    } else if (0x0A <= buf_l && buf_l <= 0x0F) {
      Sendbuf[2 * i + 2] = buf_l - 0x0A + 'A';
    }
  }
  // UartWrite_WirelessModule(Sendbuf, lenbuf);

  UartWrite_ServoMotor(Sendbuf, lenbuf);
}

void Uart_ReadWriteRegister_ServoMotor(uint8_t Read_WriteFunc, uint8_t WordsNum, uint16_t Register, uint16_t Write_Once_ByteData, uint8_t *Write_Multi_Bytes) {
  uint8_t UartSendbuf[255] = {0};
  uint8_t lenbuf           = 0;
  UartSendbuf[0]           = ServoMotor.address;
  UartSendbuf[1]           = Read_WriteFunc;
  UartSendbuf[2]           = Register >> 8;
  UartSendbuf[3]           = Register & 0xFF;
  switch (Read_WriteFunc) {
    case Cmd_Read_Func_ServoMotor: {  // 读
      lenbuf         = 6;
      UartSendbuf[4] = 0x00;
      UartSendbuf[5] = WordsNum;
    } break;
    case Cmd_Write_Once_Func_ServoMotor: {  // 写单组
      lenbuf         = 6;
      UartSendbuf[4] = Write_Once_ByteData >> 8;
      UartSendbuf[5] = Write_Once_ByteData & 0xFF;
    } break;
    case Cmd_Write_Multi_Func_ServoMotor: {
      lenbuf         = 7 + 2 * WordsNum;
      UartSendbuf[4] = 0x00;
      UartSendbuf[5] = WordsNum;
      UartSendbuf[6] = 2 * WordsNum;
      for (uint8_t i = 0; i < 2 * WordsNum; i++) {
        UartSendbuf[7 + i] = Write_Multi_Bytes[i];
      }
    } break;
    default:
      return;
  }
  ServoMotor.UartCurrentCmd[0] = Read_WriteFunc;
  ServoMotor.UartCurrentCmd[1] = Register;
  ServoMotor.UartCurrentCmd[2] = WordsNum;
  UartWrite_HexToAscii_AddLRC_ServoMotor(UartSendbuf, lenbuf);
}

void SetStatus_Power_ServoMotor(uint8_t status) {
  HAL_GPIO_WritePin(Power_ServoMotor_GPIO_Port, Power_ServoMotor_Pin, status);
  ServoMotor.IsPowerOn = Power_ServoMotor_Pin == PowerON_ServoMotor ? 1 : 0;
}
uint8_t GetStatus_Power_ServoMotor(void) { return ServoMotor.IsPowerOn; }

void UartReceiveIT_ServoMotor(void) {                                       // 读写器串口接收
  HAL_UART_Receive_IT(&huart_ServoMotor, (uint8_t *)Rxdbuf_ServoMotor, 1);  // 重新启动接收中断
}
void UartGetData_ServoMotor(void) {  // 处理接收到的数据到新的数组，并重新接收

  UartRegroup_ASCII_ServoMotor();

  UartReceiveIT_ServoMotor();
  //	UartRxd_ServoMotor[CntRxd_ServoMotor] = Rxdbuf_ServoMotor[0];
  //	CntRxd_ServoMotor++;
}

uint8_t Get_Regroup_ASCII(uint8_t data, uint8_t H_L) {
  uint8_t databuf;
  if ('0' <= data && data <= '9') {
    databuf = data - '0';
  } else if ('A' <= data && data <= 'F') {
    databuf = data - 'A' + 0x0A;
  }
  if (H_L == 0) {
    return databuf << 4;
  } else if (H_L == 1) {
    return databuf;
  }
  return 0;
}
/****************************自己的***************************************/
void moteruart_receive(void) {
  uint8_t  len    = 0;
  uint8_t  lenbuf = 0;
  uint8_t  buf[255];
  uint8_t  uartbuf[255];
  uint32_t mm;
  // LOGI("SUCESS UART MOTOR");

  len    = UartRead_ServoMotor(buf, sizeof(buf));
  lenbuf = len / 2;
  for (uint8_t i = 0; i < lenbuf; i++) {
    uartbuf[i] = Get_Regroup_ASCII(buf[2 * i], 0) | Get_Regroup_ASCII(buf[2 * i + 1], 1);
  }

  if (uartbuf[0] != ServoMotor.address && uartbuf[1] != (uint8_t)(ServoMotor.UartCurrentCmd[0] & 0xFF)) {
    LOGI("address error ");
    return;
  }

  switch (uartbuf[1]) {
    case Cmd_Read_Func_ServoMotor:
      motor_state_reg[0] = uartbuf[3];
      motor_state_reg[1] = uartbuf[4];
      motor_state_reg[2] = uartbuf[5];
      motor_state_reg[3] = uartbuf[6];

      break;
    case Cmd_Write_Once_Func_ServoMotor:
      motor_ack[0] = uartbuf[4];
      motor_ack[1] = uartbuf[5];
      break;
    case Cmd_Write_Multi_Func_ServoMotor:
      break;
  }

  // for (int i = 0; i < 8; i++)  //
  //   LOGI("data is %02x", uartbuf[i]);
  // mm = get_Position_mm(motor_state_reg, 4);
  // LOGI("distance is %d", mm);
}

/*******************************************************************/

void UartRegroup_ASCII_ServoMotor(void) {
  // static bool clear_falg;
  if (Flag_UartRegroup_ASCII_ServoMotor == 2) {
    Flag_UartRegroup_ASCII_ServoMotor = Rxdbuf_ServoMotor[0] == 0x0D ? 3 : 1;
  }
  switch (Flag_UartRegroup_ASCII_ServoMotor) {
    case 0: {  // 帧头必须是':'(0x3A)
      if (Rxdbuf_ServoMotor[0] == ':') Flag_UartRegroup_ASCII_ServoMotor = 1;
    } break;
    case 1: {  // 高字节
      UartRxd_ServoMotor[CntRxd_ServoMotor++] = Rxdbuf_ServoMotor[0];
      Flag_UartRegroup_ASCII_ServoMotor       = 2;
    } break;
    case 2: {  // 低字节
      UartRxd_ServoMotor[CntRxd_ServoMotor++] = Rxdbuf_ServoMotor[0];
    } break;
    case 3: {  // 0x0D
      Flag_UartRegroup_ASCII_ServoMotor = 4;
      if (ServoMotor.Flag_Event[2] >= 16) {
        // if (!clear_falg) {
        //   memset(UartRxd_ServoMotor, 0, 255);
        //   clear_falg = true;
        // }
        moteruart_receive();
        // enterQueueEvent(EVENT_MotorUart);
        Servo_info.uart_recv_state = true;
        // LOGI("EVENT MOTOR");
      }
    } break;
    case 4: {  // 0x0A
      Flag_UartRxdMonitor_ServoMotor = 1;
    } break;
  }
}

void UartWrite_ServoMotor(uint8_t *pData, uint16_t Size) {  // 发送给读写器
  HAL_UART_Transmit(&huart_ServoMotor, pData, Size, 1000);
}
uint8_t UartRead_ServoMotor(uint8_t *buf, uint8_t len) {
  uint8_t i;
  if (len > CntRxd_ServoMotor) {
    len = CntRxd_ServoMotor;
  }
  for (i = 0; i < len; i++) {
    buf[i] = UartRxd_ServoMotor[i];
  }
  CntRxd_ServoMotor                 = 0;
  Flag_UartRegroup_ASCII_ServoMotor = 0;
  return len;
}
void UartRxdMonitor_ServoMotor(uint8_t time_ms) {
  static uint8_t cnt_bkp    = 0;
  static uint8_t idle_timer = 0;
  if (CntRxd_ServoMotor > 0) {
    if (cnt_bkp != CntRxd_ServoMotor) {
      cnt_bkp    = CntRxd_ServoMotor;
      idle_timer = 0;
    } else {
      if (idle_timer < 10) {
        idle_timer += time_ms;
        if (idle_timer >= 10) {
          CntRxd_ServoMotor                 = 0;
          Flag_UartRegroup_ASCII_ServoMotor = 0;
          Flag_UartRxdMonitor_ServoMotor    = 1;
        }
      }
    }
  } else {
    cnt_bkp = 0;
  }
}
void UartDriver_ServoMotor(void) {
  uint8_t len;
  uint8_t buf[255] = {0};
  if (Flag_UartRxdMonitor_ServoMotor) {
    Flag_UartRxdMonitor_ServoMotor = 0;
    len                            = UartRead_ServoMotor(buf, sizeof(buf));
    UartAction_ServoMotor(buf, len);
  }
}
