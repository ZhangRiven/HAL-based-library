/*
 * @Author       : ZhangRiven
 * @Date         : 2023-08-02 12:40:32
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-04 12:54:33
 * @FilePath     : \HAL_F407ZGT6\Application\adc_analyze.hpp
 * @Description  : 
 */
#ifndef __ADC_ANALYZE_HPP
#define __ADC_ANALYZE_HPP


#include "adc_buffer.hpp"
#include "waveform_print.hpp"
#include "adc.h"
#include "tim.h"

extern adc_buffer<u16> adc_buffer1;
extern waveform_print<u16> wave_print;

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t adc2_ConvCplt;
void adc_Init();
void adc_start(); // 启动adc、dma
void adc_fm_all_in(float *dfmax);

#ifdef __cplusplus
}
#endif


#endif