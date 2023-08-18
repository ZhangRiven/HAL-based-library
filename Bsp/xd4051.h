/*
 * @Author       : ZhangRiven
 * @Date         : 2023-08-04 02:20:18
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-04 04:04:32
 * @FilePath     : \HAL_F407ZGT6\Bsp\xd4051.h
 * @Description  :
 */
#ifndef __XD4051_h
#define __XD4051_h
#include "task_app.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  GPIO_TypeDef *Port[4];//a/b/c/inh
  uint16_t Pin[4];
} xd4051_def;

extern xd4051_def xd4051;

void xd4051_swtich_channel(xd4051_def *chip, uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif