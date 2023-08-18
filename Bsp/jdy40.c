/*
 * @Author       : ZhangRiven
 * @Date         : 2023-07-31 10:20:49
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-07-31 11:48:32
 * @FilePath     : \HAL_F407ZGT6\Bsp\jdy40.c
 * @Description  :
 */
#include "jdy40.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"

#define SET(x) HAL_GPIO_WritePin(jdy40_SET_GPIO_Port, jdy40_SET_Pin, x)

void send_char(char *s, uint32_t len) {
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)s, len);
}

void read_char(char *s, uint32_t len) {
  HAL_UART_Receive_DMA(&huart5, (uint8_t *)s, len);
}

char send_buffer[16];
char read_buffer[16];
const char *AT = "AT+";      // 包头
const char *BAUD = "BAUD";   // 波特率
const char *RFID = "RFID";   // 无线ID
const char *DVID = "DVID";   // 设备ID
const char *RFC = "RFC";     // 频道
const char *POWE = "POWE";   // 发送功率
const char *CLASS = "CLASS"; // 设备类型
const char *end = "\r\n";    // 行尾
// TODO:未验证

int send_config_cmd(char *cmd) { // 配置成功返回0
  SET(1);
  // 发送AT指令
  send_char(cmd, strlen(cmd));

  // 接收响应
  uint8_t resp[32];
  read_char(read_buffer, 2);
  SET(0);
  // 判断响应是否为"OK"
  return strcmp(read_buffer, "OK");
}

/* 选参数配置 */
int Set_jdy40(const char *s, uint8_t n) {
  sprintf(send_buffer, "%s%s%d%s", AT, s, n, end);
  return send_config_cmd(send_buffer);
}

/* 1:1200
2:2400
3:4800
4:9600
5:14400
6:19200
默认值：4 */
int Set_BAUD(uint8_t baud_n) {
  baud_n = 1200 * 2 ^ (baud_n - 1);
  return Set_jdy40(BAUD, baud_n);
}

/* (0000-FFFF)默认值：8899 */
int Set_RFID(uint8_t n) { return Set_jdy40(RFID, n); }

/* (0000-FFFF)默认值：8899 */
int Set_DVID(uint8_t n) { return Set_jdy40(DVID, n); }

/* Param(001-128)默认值：001 */
int Set_RFC(uint8_t n) { return Set_jdy40(RFC, n); }

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
int Set_POWE(uint8_t n) { return Set_jdy40(POWE, n); }

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
int Set_CLASS(const char *s) {
  sprintf(send_buffer, "%s%s%s%s", AT, CLASS, s, end);
  return send_config_cmd(send_buffer);
}