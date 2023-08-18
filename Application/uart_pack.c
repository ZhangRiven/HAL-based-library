#include "uart_pack.h"

#include "string.h"
#include "usart.h"
#include <stdint.h>

UART_HandleTypeDef *huart = &huart1; // 串口句柄
const char *sync = "$";              // 同步信号（包头）定义

  // dma方式需要调用UART_receive_data后在完成回调函数中调用find_pack()处理
int UART_receive_data(char *s, uint16_t len) {
  return HAL_UART_Receive_DMA(huart, (uint8_t *)s, len);
}

int UART_send_data(char *s, uint16_t len) {
  return HAL_UART_Transmit_DMA(huart, (uint8_t *)s, len);
}

/*
中断或DMA方式：
1、调用receive_data_IT();
2、在中断中调用find_pack();

阻塞方式：
1、调用receive_data();
2、返回值为0时，解析数据包
*/



uart_receive_type receive_buffer;                  // 一帧数据包
const uint32_t len = sizeof(uart_receive_type);    // 包长
char rx_tmp_buffer[2 * sizeof(uart_receive_type)]; // 接收缓冲

uint32_t find_pack() { // 遍历到包头后拷贝一帧的包数据
  for (uint8_t i = 0; i < 2 * sizeof(uart_receive_type); i++) {
    if (sync[0] == rx_tmp_buffer[i]) {
      memcpy(&receive_buffer, &rx_tmp_buffer[i], len);
      return 0;
    }
  }
  return 1;
}
uint32_t receive_data_IT() { return UART_receive_data(rx_tmp_buffer, 32); }

// 串口接收数据包 阻塞用法
uint32_t receive_data() {
  if (0 == UART_receive_data(rx_tmp_buffer, 32)) {
    return find_pack();
  } else {
    return 1;
  }
}

// 串口发送数据包
uint32_t send_data() { // 发送两次
  return UART_send_data((char *)&receive_buffer, len);
}

void write_data(const char *s) { memcpy(receive_buffer.str, s, strlen(s)); }

char *read_data() { return receive_buffer.str; }
