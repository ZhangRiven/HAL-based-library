/*
 * @Author       : ZhangRiven
 * @Date         : 2023-04-25 20:17:09
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-07-29 10:06:18
 * @Description  :
 */
#ifndef __PRINTF_DMA_H__
#define __PRINTF_DMA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
extern  uint8_t usart_dma_tx_over;
int printf_DMA(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif