/*
 * @Author       : ZhangRiven
 * @Date         : 2023-07-31 10:01:10
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-03 08:33:41
 * @FilePath     : \HAL_F407ZGT6\Application\task_key.h
 * @Description  : 
 */
#ifndef __TASK_KEY
#define __TASK_KEY
#include "task_app.hpp"
#include "KeyPad.hpp"

using bsp::KeyPad;
extern KeyPad<GPIO_TypeDef *, uint16_t> keypad;
void timer_Key_Scan_Init();
void Key_scan_Timer_Callback(void *argument);
#endif