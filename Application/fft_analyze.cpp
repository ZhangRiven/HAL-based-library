/*
 * @Author       : ZhangRiven
 * @Date         : 2023-08-02 16:47:46
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-06 09:49:11
 * @FilePath     : \HAL_F407ZGT6\Application\fft_analyze.cpp
 * @Description  :
 */
#include "fft_analyze.hpp"
#include "dsp/window_functions.h"
#include "signal_analyze.h"
#include "stm32_dsp.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>


using std::strlen;
const uint32_t fs = 64;       // 采样频率kHz
const uint32_t fs2 = 1024;    // 采样频率kHz
const uint16_t fftlen = 1024; // 记得改fft的因子
static float32_t Input_f32[fftlen * 2];
float32_t Output_f32[fftlen];

/* 绘图 */
waveform_print<float> fft_wave_print(0, 3.3);

void fft_Init() {
  fft_wave_print.Set_buffer(Output_f32, fftlen / 2 - 1)
      .Set_Print_window(0, 140, 319, 239); // 设置窗口
}

float get_vpp_and_triple() {
  float max = 0, min = 4095;
  for (uint16_t index = 0; index < fftlen; index++) {
    if (adc_buffer1.Get_data(index + 100) > max) {
      max = adc_buffer1.Get_data(index + 100);
    }
    if (adc_buffer1.Get_data(index + 100) < min) {
      min = adc_buffer1.Get_data(index + 100);
    }
  }
  float vpp = (max - min) *3.3/ 4095.0;
  if (vpp <= 1) {
    vpp*=3.1;
    triple(GPIO_PIN_RESET);
  } else {
    triple(GPIO_PIN_SET);
  }
  return vpp;
}
void fft_mag_cacluate() {
  for (uint32_t i = 0; i < fftlen; i++) {
    Input_f32[i * 2] = 3.3 * adc_buffer1.Get_data(i + 100) / 4095.0;
    Input_f32[i * 2 + 1] = 0;
  }
  // arm_hamming_f32(Input_f32, fftlen);
  arm_cfft_f32(&arm_cfft_sR_f32_len1024, Input_f32, 0, 1);
  arm_cmplx_mag_f32(Input_f32, Output_f32, fftlen);

  /* 求出实际大小 */
  Output_f32[0] = Output_f32[0] / fftlen;
  for (uint32_t i = 1; i < fftlen; i++) {
    Output_f32[i] = Output_f32[i] / (fftlen / 2.0);
  }
}

// fft串口上传
void fft_uart_print() {
  for (uint32_t i = 0; i < fftlen; i++) {
    static char str[32];
    sprintf(str, "%.1f,%d,%.3f\n", 3.3 * adc_buffer1.Get_data(i) / 4095.0,
            fftlen - i, Output_f32[i]);
    HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
  }
}
// fft串口上传
void fft_uart_print(float *data, uint16_t len) {
  for (uint32_t i = 0; i < len; i++) {
    static char str[32];
    sprintf(str, "%d,%.3f\n", len - i, data[i]);
    HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
  }
}
uint16_t FM_poit_buffer[4096]; // fm专用fft数组
float32_t FM_fft_input[8192];
float32_t FM_fft_output[4096];

void data_normalized(uint16_t *adc, float32_t *out_buffer, uint16_t adc_len) {
  for (uint16_t i = 0; i < adc_len; i++) {
    out_buffer[i * 2] = 3.3 * adc[i] / 4095.0;
    out_buffer[i * 2 + 1] = 0;
  }
}

void cacluate_fft_mag_4096(float32_t *input, float32_t *output, uint16_t len) {

  arm_cfft_f32(&arm_cfft_sR_f32_len4096, input, 0, 1);
  arm_cmplx_mag_f32(input, output, len);

  /* 求出实际大小 */
  output[0] = output[0] / len;
  for (uint32_t i = 1; i < len; i++) {
    output[i] = output[i] / (len / 2.0);
  }
}

float findmx(float *fft_outputbuf) {
  // 找最大值，非清零
  u16 keep, i;
  float temp;
  temp = fft_outputbuf[1];
  for (i = 1; i < 2048; i++) {
    if (temp < fft_outputbuf[i]) {
      keep = i;
      temp = fft_outputbuf[i];
    }
  }
  return keep;
}

u16 pingpian = 0;
u16 pingpian_k = 0, pingpian_e = 0;
u16 find_start = 0;
#define jiange 90

/************************        找波峰波谷                     *******/
u16 papoyou(float *fft_outputbuf) {
  u16 bofeng, bogu;
  while (fft_outputbuf[find_start + 1] > fft_outputbuf[find_start]) {
    find_start++;
  }
  bofeng = find_start;

  pingpian_e = bofeng; // find_start++;
  while (fft_outputbuf[find_start + 1] < fft_outputbuf[find_start]) {
    find_start++;
  }
  bogu = find_start;

  return 1;
}

u16 papozuo(float *fft_outputbuf) {
  u16 bofeng, bogu;
  while (fft_outputbuf[find_start - 1] > fft_outputbuf[find_start]) {
    find_start--;
  }
  bofeng = find_start;
  // find_start--;
  pingpian_k = find_start;
  while (fft_outputbuf[find_start - 1] < fft_outputbuf[find_start]) {
    find_start--;
  }
  bogu = find_start;
  return 1;
}

u16 zuoyoufind(float *fft_outputbuf) {
  uint16_t centor, nm = 0, jiange_num = 0; // F=0
  float yuzhi_auto;
  /*************向右找*****************/
  uint16_t over_thr_n;
  //  find_main_lobe(FM_fft_output, 4096, &over_thr_n, &centor);
  centor = findmx(fft_outputbuf);
  find_start = centor;
  yuzhi_auto = fft_outputbuf[centor] * 0.5;
  while (jiange_num < jiange) {
    if (fft_outputbuf[find_start] > yuzhi_auto) {
      //				 pingpian_e=find_start;
      while (!papoyou(fft_outputbuf))
        ;
      nm += 1;
      jiange_num = 0;
    }
    jiange_num++;
    //	if(fft_outputbuf[find_start]<yuzhi) {}
    find_start++;
    if (find_start >= 2048)
      break;
  }
  ///****************向左找*********************/
  jiange_num = 0;
  find_start = centor;
  while (jiange_num < jiange) {
    if (fft_outputbuf[find_start] > yuzhi_auto) {
      //				 pingpian_k=find_start;
      while (!papozuo(fft_outputbuf))
        ;
      nm += 1;
      jiange_num = 0;
    }
    jiange_num++;
    // printf("%d\r\n",find_start);
    if (find_start <= 0)
      break;
    find_start--;
  }
  //****************算频偏*****************************/
  pingpian = pingpian_e - pingpian_k;

  return 1;
}

float fm_get() {
  // char zhongjian1[1024];
  // char zhongjian3[1024];
  // uint16_t i=0;
  //	float j;
  //  //  printf("fm\r\n");
  //	LCD_Clearline(YELLOW,3)	;
  //	Show_Str(0,48,BLUE,YELLOW,  "FM",16,1);
  zuoyoufind(FM_fft_output);  // 找峰值
  return (pingpian)*0.25 / 2; // 频偏
  // mf=(float) (F/(2*freq_measure) - 1);
  // printf("fd=%.2lf,mf=%.2lf",F,mf);
}