/*
 * @Author       : ZhangRiven
 * @Date         : 2023-08-02 12:43:17
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-06 04:58:22
 * @FilePath     : \HAL_F407ZGT6\Application\adc_analyze.cpp
 * @Description  :
 */

#include "adc_analyze.hpp"
#include "fft_analyze.hpp"
#include "signal_analyze.h"
uint8_t adc2_ConvCplt = 0;
adc_buffer<u16> adc_buffer1(fftlen + 200);
waveform_print<u16> wave_print(0, 4095);
void adc_Init() {
  fft_Init();                // fft库初始化
  adc_buffer1.Init_Buffer(); // 初始化缓存
  wave_print.Set_buffer(adc_buffer1.Get_buffer_ptr(), 500)
      .Set_Print_window(0, 140, 319, 239); // 设置窗口
  HAL_TIM_Base_Start(&htim3);             // 启动定时器
  HAL_TIM_Base_Start(&htim8);             // 启动定时器
}

/**
 * @description: 启动adc、dma
 * @return {*}
 */
void adc_start() {
  HAL_ADC_Start_DMA(
      &hadc1, reinterpret_cast<u32 *>(adc_buffer1.Get_buffer_ptr()),
      adc_buffer1.Get_Sampling_number()); // 启动ADC采集存放到buff数组里
}

void adc_fm_start() {
  HAL_ADC_Start_DMA(&hadc2, (uint32_t *)FM_poit_buffer,
                    4096); // 启动ADC采集存放到buff数组里
}

/* 什么都有（真的写吐了5555） */
void adc_fm_all_in(float *dfmax) {
  uint32_t tick = 0;
  adc_fm_start();
  tick = HAL_GetTick();
  data_normalized(FM_poit_buffer, FM_fft_input, 4096);
  cacluate_fft_mag_4096(FM_fft_input, FM_fft_output, 4096); // 计算fft和幅值
  uint16_t over_thr_n, main_lobe_i;
  find_main_lobe(FM_fft_output, 4096, &over_thr_n, &main_lobe_i); // 找到主瓣
  *dfmax = max_freq_dev(&main_lobe_i, FM_fft_output, 4096, fs2);                         // 求dfmax

  // fft_uart_print(FM_fft_output,4096);
  // *dfmax = fm_get();
  // wave_print.Set_pen_color(LCD::COLOR_16BIT::GREEN).Print_wave();
  // fft_wave_print.Set_pen_color(LCD::COLOR_16BIT::YELLOW).Print_wave();
}