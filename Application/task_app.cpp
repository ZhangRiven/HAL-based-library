/*
 * @Author       : ZhangRiven
 * @Date         : 2023-06-19 14:04:25
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-06 09:05:31
 * @FilePath     : \HAL_F407ZGT6\Application\task_app.cpp
 * @Description  : 任务函数
 */

#include "task_app.hpp"

#include "AD9851.h"
#include "AD9954.h"
#include "adc.h"
#include "adc_analyze.hpp"
#include "arm_math.h"
#include "scan_channel.h"
#include "task_key.h"
#include "task_screen.h"
#include "tim.h"
#include "uart_pack.h"
#include "usart.h"
#include "xd4051.h"
#include <cstdint>

/* 硬件初始化 */
void hw_init() {
  timer_Key_Scan_Init();
  AD9851_Init();
  AD9851_Write(AD9851_freqword(2660000));
  // AD9954_Init();
  // AD9954_SETFRE(200000);
}

void screenTask(void *argument);

/* 总初始化 */
void task_Init() {
  hw_init();
  // printf_DMA("hw finished!\n");
  channel_fun_Init();
  adc_Init();
  task_screen_flush_Init();
  delay_ms(500);
  xd4051_swtich_channel(&xd4051, 0);
}

uint32_t adc_scan_tick = 1000;
/* 任务循环 */
void task_while() {
  static u32 tick = 0;
  if (HAL_GetTick() - tick >= adc_scan_tick) {
    screen.Show_string(10, 90, 300, 16, "Press 1 start scan.",
                       LCD::FONT_TYPE_SIZE::_16, LCD::COLOR_16BIT::WHITE, 1,
                       LCD::COLOR_16BIT::BLACK);
    if (keypad.Get_Key_Value() == 1) {
      while (0 == scan_channel()) { // 0无波形连续扫，2单次扫描
        screen.Show_string(10, 50, 120, 16, "scaning", LCD::FONT_TYPE_SIZE::_16,
                           LCD::COLOR_16BIT::WHITE, 1, LCD::COLOR_16BIT::BLACK);
      }
    }
    tick = HAL_GetTick();
  }
  screenTask(nullptr);
}

/* 串口发送完成中断 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  if (&huart1 == huart) {
    usart_dma_tx_over = 1; // 释放互斥锁
  }
}

/**
 * @description: DMA触发的一轮ADC完成回调
 * @param {ADC_HandleTypeDef} *hadc
 * @return {*}
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
  if (&hadc1 == hadc) {
    adc_buffer1.Set_finished_flag();
  }
  if (&hadc2 == hadc) {
    adc2_ConvCplt = 1;
  }
}

/* 按键扫描 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (&htim7 == htim) {

    keypad.Scan_callback_handler();
  }
}
