/*
 * @Author       : ZhangRiven
 * @Date         : 2023-08-01 23:43:16
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-02 21:16:30
 * @FilePath     : \HAL_F407ZGT6\Application\dsp.c
 * @Description  :
 */

#include "dsp.h"
#include "math.h"
#include "printf_dma.h"
#include "arm_math.h"
#include "stm32_dsp.h"
uint32_t input[1024], output[1024], Mag[1024]; /* 输入，输出和幅值 */
float Phase[1024];                         /* 相位*/
/*
******************************************************************
* 函 数 名: PowerMag
* 功能说明: 求模值
* 形 参：_usFFTPoints FFT点数
* 返 回 值: 无
******************************************************************
*/
void PowerMag(uint16_t _usFFTPoints) {
  int16_t lX, lY;
  uint16_t i;
  float mag;
  /* 计算幅值 */
  for (i = 0; i < _usFFTPoints; i++) {
    lX = (output[i] << 16) >> 16;                       /* 实部*/
    lY = (output[i] >> 16);                             /* 虚部 */
    arm_sqrt_f32((float)(lX * lX + lY * lY), &mag); /* 求模 */
    Mag[i] = mag * 2; /* 求模后乘以2才是实际模值，直流分量不需要乘2 */
  }

  /* 由于上面多乘了2，所以这里直流分量要除以2 */
  Mag[0] = Mag[0] >> 1;
}
/*
******************************************************************
* 函 数 名: Power_Phase_Radians
* 功能说明: 求相位
* 形 参：_usFFTPoints FFT点数， uiCmpValue 阀值
* 返 回 值: 无
******************************************************************
*/
void Power_Phase_Radians(uint16_t _usFFTPoints, uint32_t _uiCmpValue) {
  int16_t lX, lY;
  uint16_t i;
  float phase;
  float mag;
  for (i = 0; i < _usFFTPoints; i++) {
    lX = (output[i] << 16) >> 16; /* 实部 */
    lY = (output[i] >> 16);       /* 虚部 */
    phase = atan2(lY, lX); /* atan2求解的结果范围是(-pi, pi], 弧度制 */
    arm_sqrt_f32((float)(lX * lX + lY * lY), &mag); /* 求模 */
    if (_uiCmpValue > mag) {
      Phase[i] = 0;
    } else {
      Phase[i] = phase * 180.0f / 3.1415926f; /* 将求解的结果由弧度转换为角度 */
    }
  }
}
/*
********************************************************************
* 函 数 名: DSP_FFTPhase
* 功能说明: 1024点FFT的相位求解
* 形 参：无
* 返 回 值: 无
********************************************************************
*/
void DSP_FFTPhase(void) {
  uint16_t i;
  /* 获得1024个采样点 */
  for (i = 0; i < 1024; i++) {
    /* 波形是由直流分量，50Hz正弦波组成，波形采样率1024 */
    input[i] =
        1024 + 1024 * cos(2 * 3.1415926f * 50 * i / 1024 + 3.1415926f / 3);
  }
  /* 计算1024点FFT
   output：输出结果，高16位是虚部，低16位是实部。
   input ：输入数据，高16位是虚部，低16位是实部。
   第三个参数必须是1024。
  */
  cr4_fft_1024_stm32(output, input, 1024);
  /* 求幅值 */
  PowerMag(1024);
  /* 打印输出结果 */
  for (i = 0; i < 1024; i++) {
    printf_DMA("%d\r\n", Mag[i]);
  }
  /* 求相频 */
  Power_Phase_Radians(1024, 100);
  /* 打印输出结果 */
  for (i = 0; i < 1024; i++) {
    printf_DMA("%f\r\n", Phase[i]);
  }
}

