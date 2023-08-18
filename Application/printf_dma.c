/*
 * @Author       : ZhangRiven
 * @Date         : 2023-07-30 16:22:55
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-02 19:32:29
 * @FilePath     : \HAL_F407ZGT6\Application\printf_dma.c
 * @Description  :
 */
#include "printf_dma.h"
#include "usart.h"
#include <stdarg.h>
#include <stdio.h>

uint8_t usart_dma_tx_over = 1;
int printf_DMA(const char *format, ...) {
  while (!usart_dma_tx_over)
    ; // 等待前一次DMA发送完成
  va_list arg;
  char SendBuff[64] = {0};
  int rv;
  usart_dma_tx_over = 0; // 清0全局标志，发送完成后重新置1
  va_start(arg, format);
  rv = vsnprintf((char *)SendBuff, sizeof(SendBuff), (char *)format, arg);
  va_end(arg);
  HAL_UART_Transmit_IT(&huart1, (uint8_t *)SendBuff, rv);

  return rv;
}

int fputc(int ch, FILE *f) {
  uint8_t temp[1] = {ch};
  HAL_UART_Transmit(&huart1, temp, 1, 0xff);
  return ch;
}
