#ifndef __ADC_ANALYZE_H
#define __ADC_ANALYZE_H
#include "stdint.h"
#include "adc_analyze.hpp"
#include "arm_const_structs.h"
#include "arm_math.h"

extern waveform_print<float> fft_wave_print;

#ifdef __cplusplus
extern "C" {
#endif
extern const uint32_t fs;
extern const uint32_t fs2;
extern const uint16_t fftlen;
extern float32_t Output_f32[];
void fft_Init();
void fft_mag_cacluate();
extern uint16_t FM_poit_buffer[]; 
extern float32_t FM_fft_input[];
extern float32_t FM_fft_output[];
void data_normalized(uint16_t *adc, float32_t *out_buffer, uint16_t adc_len);
void cacluate_fft_mag_4096(float32_t *input, float32_t *output, uint16_t len);
#ifdef __cplusplus
}
#endif
#define triple(x) HAL_GPIO_WritePin(npn_GPIO_Port, npn_Pin, x)

float get_vpp_and_triple();
void fft_uart_print();
void fft_uart_print(float *data, uint16_t len);
float fm_get();
#endif