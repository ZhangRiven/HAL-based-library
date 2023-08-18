/*
 * @Author       : ZhangRiven
 * @Date         : 2023-08-04 05:17:19
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-08 09:41:56
 * @FilePath     : \HAL_F407ZGT6\Application\scan_channel.cpp
 * @Description  :
 */
#include "scan_channel.h"
#include "adc_analyze.hpp"
#include "adc_buffer.hpp"
#include "fft_analyze.hpp"
#include "signal_analyze.h"
#include "xd4051.h"
#include <stdint.h>
#include "stdio.h"
void (*channel_fun[4])(uint8_t);

void channel_0(uint8_t wave_type) {
  if (1 == wave_type) { // sine
    cacluate_AM_Ma();
    
  } else if (2 == wave_type) { // square
    cacluate_2ASK();
  }
}

void channel_1(uint8_t wave_type) {
  if (1 == wave_type) { // sine
    cacluate_FM_df_Mf();
  } else if (2 == wave_type) { // square
    cacluate_2FSK_Rc_h();
  }
}

void channel_2(uint8_t wave_type) {
  if (1 == wave_type) { // sine
    cacluate_CW();
  } else if (2 == wave_type) { // square
    No_sign();
  }
}

void channel_3(uint8_t wave_type) {
  if (1 == wave_type) { // sine
    No_sign();
  } else if (2 == wave_type) { // square
    cacluate_2PSK();
  }
}

void channel_fun_Init() {
  channel_fun[0] = channel_0;
  channel_fun[1] = channel_1;
  channel_fun[2] = channel_2;
  channel_fun[3] = channel_3;
}

uint8_t scan_channel() {
  uint32_t tick = 0;
  No_sign();
  for (uint8_t ch = 0; ch < 4; ch++) { // TODO:应从0开始,小于4,每次+1
    triple(GPIO_PIN_SET);
    xd4051_swtich_channel(&xd4051, ch); // 模拟开关选通
    delay_ms(200);
    adc_start();
    tick = HAL_GetTick();
    while (0 == adc_buffer1.Get_finished_flag()) { // 等待转换
    }
    adc_buffer1.Reset_finished_flag();
    fft_mag_cacluate(); // 计算fft和幅值
    uint8_t wave_type = check_wave(ch); // 分析波形
    wave_print.Clear().Set_pen_color(LCD::COLOR_16BIT::RED).Print_wave();
    fft_wave_print.Set_pen_color(LCD::COLOR_16BIT::BLUE).Print_wave();
    if (wave_type == 0) { // 没有波形，扫下一个通道
      continue;
    } else { // 有波形，处理数据
      if (channel_fun[ch] != nullptr) {
        channel_fun[ch](wave_type);
        char str[16];
        float vpp = get_vpp_and_triple();
        sprintf(str, "vpp:%.1fV",vpp);
        
        screen.Show_string(10, 120, 100, 16, str, LCD::FONT_TYPE_SIZE::_16,
                       LCD::COLOR_16BIT::RED, 1, LCD::COLOR_16BIT::BLACK);
      }
      break; // 找到，退出for循环
    }
  }
    // fft_uart_print();
  return signal_type_screen_print(); // 打印调制参数,返回是否有波形
}