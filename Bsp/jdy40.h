/*
引脚功能描述
引脚 管脚名称 功能
1 GPIO1 输入输出 IO，可通过 AT+CLSS 配置
2 GPIO2 输入输出 IO，可通过 AT+CLSS 配置
3 GPIO3 输入输出 IO，可通过 AT+CLSS 配置
4 GPIO4 输入输出 IO，可通过 AT+CLSS 配置
5 GPIO5 输入输出 IO，可通过 AT+CLSS 配置
6 GPIO6 输入输出 IO，可通过 AT+CLSS 配置
7 GPIO7 输入输出 IO，可通过 AT+CLSS 配置
8 GPIO8 输入输出 IO，可通过 AT+CLSS 配置
9 VCC 电源（2.2V–3.6V）
10 RXD 串口输入引脚
11 TXD 串口输出引脚
12 SET AT 指令切换引脚（低电平 AT 指令，高电平透传）
13 CS 透传或按键发射模式：CS 片选引脚（低电平唤醒，高电平睡眠）、IO
接收模式：需要将 CS 引脚悬空或拉高电平 14 GND 电源地
*/
#ifndef __JDY40_H
#define __JDY40_H
#include "task_app.h"
#ifdef __cplusplus
extern "C" {
#endif

/* 1:1200
2:2400
3:4800
4:9600
5:14400
6:19200
默认值：4 */
int Set_BAUD(uint8_t baud_n);

/* (0000-FFFF)默认值：8899 */
int Set_RFID(uint8_t n);

/* (0000-FFFF)默认值：8899 */
int Set_DVID(uint8_t n);

/* Param(001-128)默认值：001 */
int Set_RFC(uint8_t n);

/* Param(0-9)
0：-25db
1：-15db
2：-5db
3：0db
4：+3db
5：+6db
6：+9db
7：+10db
8：+10db
9：+12db
默认值：9 */
int Set_POWE(uint8_t n);

/* Param
A0：串口透传（收发一体）
C0：遥控器或 IO 按键带指示灯（发射端）
C1：遥控器或 IO 按键不带指示灯（发射端）
C2：IO 平时高电平，收到信号后低电平，延
 时 30mS 后恢复高电平
C3：IO 平时低电平，收到信号后高电平，已
 经延时 30mS 后恢复低电平
C4：IO 平时低电平，收到按下信号高电平，
 收到抬起信号低电平
C5：IO 接收到按下信号时 IO 电平反向
默认值：A0 */
int Set_CLASS(const char *s);

#ifdef __cplusplus
}
#endif
#endif