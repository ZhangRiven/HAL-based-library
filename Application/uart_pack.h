/*
 * @Author       : ZhangRiven
 * @Date         : 2023-07-31 00:46:25
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-01 08:48:44
 * @FilePath     : \HAL_F407ZGT6\Application\uart_pack.h
 * @Description  :
 */
#ifndef _UART_PACK_H
#define _UART_PACK_H
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __attribute__((packed)) {
  char sync; // 同步信号
  char str[16];
} uart_receive_type;

extern uart_receive_type receive_buffer;
uint32_t find_pack();//在串口缓存中寻找数据包
uint32_t receive_data_IT();// 串口中断接收数据包
uint32_t receive_data();        // 串口接收数据包
uint32_t send_data();           // 串口发送数据包
void write_data(const char *s); // 写包
char *read_data();              // 读包

#ifdef __cplusplus
}
#endif
#endif