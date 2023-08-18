/*
 * @Author       : ZhangRiven
 * @Date         : 2023-08-03 12:06:20
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-04 01:48:16
 * @FilePath     : \HAL_F407ZGT6\Bsp\AD9851.c
 * @Description  :
 */
#include "ad9851.h"
#include "gpio.h"

// --------兼容接口-------
// 标准库照第一个个改引脚控制api：
void FQ_UD(uint8_t level) { // UP
  if (level) {              // 高电平
    HAL_GPIO_WritePin(AD9851_UP_GPIO_Port, AD9851_UP_Pin, 1);
  } else { // 低电平
    HAL_GPIO_WritePin(AD9851_UP_GPIO_Port, AD9851_UP_Pin, 0);
  }
}

void W_CLK(uint8_t level) { // CLK
  HAL_GPIO_WritePin(AD9851_CLK_GPIO_Port, AD9851_CLK_Pin, level);
}

void D7(uint8_t level) { // DATA
  HAL_GPIO_WritePin(AD9851_DATA_GPIO_Port, AD9851_DATA_Pin, level);
  }

void gpio_init() {
  // 全部配置为 ！推挽输出！ 或开漏上拉
}
// --------兼容接口-------

// DDS初始化函数，AD9851模块reset引脚直接接地，故无法通过该引脚对AD9851复位
void AD9851_Init(void) {
  gpio_init();
  W_CLK(0);
  FQ_UD(0);
  W_CLK(1);
  W_CLK(0);
  FQ_UD(1);
}
// DDS写32位频率字、8位控制字函数
void AD9851_Write(uint64_t freqword) {
  uint8_t i, j, model = 0x01;
  FQ_UD(0);
  for (i = 0; i < 32; i++) // 先写32位的频率控制字（先低位后高位）
  {
    D7(freqword & (0x01));
    W_CLK(1);
    W_CLK(0);
    freqword >>= 1;
  }
  for (i = 0; i < 8; i++) // 再写其他8位控制字，这里只写了6倍频使能
  {
    D7(model & (0x01));
    W_CLK(1);
    W_CLK(0);
    model >>= 1;
  }

  FQ_UD(1);
}
// AD9851
// 频率控制字计算函数，由频率值freq计算频率控制字。该字与晶振（30M）直接相关//
// ftw=f0*2^32/180M，1hz对应23.861
// 注：此函数在IDE环境下验证不一定正确，但在keil环境下则正确
uint64_t AD9851_freqword(uint64_t freq) {
  uint64_t temp;
  temp = freq * 23.861;//应除以0.8
  return ((uint64_t)temp);
}