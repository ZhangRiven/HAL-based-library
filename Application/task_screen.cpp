/*
 * @Author       : ZhangRiven
 * @Date         : 2023-07-31 09:49:05
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-04 14:36:44
 * @FilePath     : \HAL_F407ZGT6\Application\task_screen.cpp
 * @Description  :
 */
#include "task_screen.h"
#include "adc_analyze.hpp"
using LCD::lcd;
using lib::StringStream;

lcd<12> screen;
StringStream<20> sstream;

inline void update_screen_with_Data_name();//显示名称
inline void flush_dot();//刷点
void screenTask(void *argument); // 刷屏任务

//osThreadId_t screenTaskHandle;
//const osThreadAttr_t screenTask_attributes = {
//    .name = "screenTask",
//    .stack_size = 128 * 8,
//    .priority = (osPriority_t)osPriorityNormal,
//};

void task_screen_flush_Init() {//刷屏初始化
  screen.Init()
      .Set_disp_dir(LCD::DISPLAY_DIRECTION::_270)
      .Clear(LCD::COLOR_16BIT::BLACK)
      .Show_string(0, 0, 200, 16, "Init finished!", LCD::FONT_TYPE_SIZE::_16,
                   LCD::COLOR_16BIT::WHITE);
  update_screen_with_Data_name();
}


void screenTask(void *argument) { // 刷屏任务
  // for (;;) {
    // static u32 tick = 0; // 刷屏时间戳
    // if (HAL_GetTick() - tick >= 50) {
    //   tick = HAL_GetTick();
    // }
    flush_dot();
    
    // delay_ms(1);
  // }
}

inline void update_screen_with_Data_name() {
  screen.Clear(LCD::COLOR_16BIT::BLACK);
  sstream.reset() << "Working";
  screen.Show_string(239, 0, 56, 16, sstream.get_str(), LCD::FONT_TYPE_SIZE::_16,
                   LCD::COLOR_16BIT::WHITE,1,LCD::COLOR_16BIT::BLACK);
  // sstream.reset() << "ADC_Freq:";
  // screen.Fill(0, 16, 72, 16, LCD::COLOR_16BIT::BLACK)
  //     .Show_string(0, 16, 72, 16, sstream.get_str(),
  //     LCD::FONT_TYPE_SIZE::_16,
  //                  LCD::COLOR_16BIT::WHITE);
}

inline void flush_dot() {
  static u32 tick = 0;
      if (HAL_GetTick() - tick >= 600) {
      static u8 n = 1; // 用于刷.
      sstream.reset();
      for (u8 i = 0; i < n; i++) {
        sstream << '.';
      }
      if (++n > 3) {
        n = 1;
      }
      screen.Show_string(295, 0, 24, 16, sstream.get_str(),
                       LCD::FONT_TYPE_SIZE::_16, LCD::COLOR_16BIT::WHITE,1,LCD::COLOR_16BIT::BLACK);
      tick = HAL_GetTick();
    }
}