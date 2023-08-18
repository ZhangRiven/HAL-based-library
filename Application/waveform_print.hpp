/*
 * @Author       : ZhangRiven
 * @Date         : 2023-06-20 17:33:53
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-04 15:08:52
 * @FilePath     : \HAL_F407ZGT6\Application\waveform_print.hpp
 * @Description  :
 */
#ifndef __WAVEFORM_PRINT_HPP
#define __WAVEFORM_PRINT_HPP
#include "lcd.hpp"
using LCD::lcd;
extern lcd<12> screen;

template <typename TYPE> class waveform_print {
public:
  waveform_print(TYPE min_value, TYPE max_value)
      : min_value(min_value), max_value(max_value) {
    pen_color = 0xF800; // 初始化为红色
  }

  /**
   * @description: 画线接口
   * @param {u16} x0
   * @param {u16} y0
   * @param {u16} x1
   * @param {u16} y1
   * @return {*}
   */
  inline waveform_print &screen_print_line(u16 x0, u16 y0, u16 x1, u16 y1) {
    screen.Draw_line(x0, y0, x1, y1, static_cast<LCD::COLOR_16BIT>(pen_color));
    return *this;
  }

  inline waveform_print &screen_print_window_Clear(u16 x0, u16 y0, u16 x1,
                                                   u16 y1) {
    screen.Fill(x0, y0, x1, y1, LCD::COLOR_16BIT::BLACK);
    return *this;
  }

  waveform_print &Set_pen_color(u32 color) {
    pen_color = color;
    return *this;
  }
  waveform_print &Set_pen_color(LCD::COLOR_16BIT color) {
    pen_color = (u16)color;
    return *this;
  }
  /**
   * @description: 传入缓存大小后分配缓存
   * @param {TYPE} *p
   * @param {u32} buffer_size
   * @return {*}
   */
  waveform_print &Buffer_Init(const u32 buffer_size) {
    if (nullptr == buffer) {
      buffer = new TYPE[buffer_size];
      _buffer_size = buffer_size;
    }
    return *this;
  }

  /**
   * @description: 析构缓存
   * @param {u32} buffer_size
   * @return {*}
   */
  waveform_print &Buffer_DeInit(const u32 buffer_size) {
    if (nullptr != buffer) {
      delete[] buffer;
    }
    return *this;
  }

  /**
   * @description: 设置指向缓存的指针、缓存大小
   * @param {TYPE} *p
   * @param {u32} buffer_size
   * @return {*}
   */
  waveform_print &Set_buffer(TYPE *buffer_ptr, u32 buffer_size) {
    buffer = buffer_ptr;
    _buffer_size = buffer_size;
    return *this;
  }

  /* 清屏 */
  waveform_print &Clear() {
    return screen_print_window_Clear(window.x0, window.y0, window.x1, window.y1);
  }

  /**
   * @description: 绘制波形
   * @return {*}
   */
  waveform_print &Print_wave() {
    // screen_print_window_Clear(window.x0, window.y0, window.x1, window.y1);

    float x_increment =
        _buffer_size / static_cast<float>(window.x1 - window.x0);
    u16 window_height = window.y1 - window.y0;
    for (u16 x = window.x0; x < window.x1; x++) {
      u16 x0 = x * x_increment;
      float y0 = (buffer[x0] - min_value) / static_cast<float>(max_value);
      x0 = (x + 1) * x_increment;
      float y1 = (buffer[x0] - min_value) / static_cast<float>(max_value);
      screen_print_line(x, window.y1 - y0 * window_height, x + 1,
                        window.y1 - y1 * window_height);
    }

    // for (u16 x = 0; x < 320; x++) {
    //   // for (u16 x = 0; x < _buffer_size - 1; x++) {
    //   float y1 = (buffer[x] - min_value) / (float)max_value;
    //   float y2 = (buffer[x + 1] - min_value) / (float)max_value;
    //   screen_print_line(x, 150 - y1 * 100, x + 1, 150 - y2 * 100);
    // }
    return *this;
  }

  waveform_print &Set_Print_window(u16 x0, u16 y0, u16 x1, u16 y1) {
    window.x0 = x0;
    window.x1 = x1;
    window.y0 = y0;
    window.y1 = y1;
    return *this;
  }

private:
  TYPE min_value;   // 最小值
  TYPE max_value;   // 最大值
  TYPE *buffer;     // 采样存储缓存
  u32 _buffer_size; // 缓存大小
  u32 pen_color;    // 画笔颜色

  struct {
    u16 x0 = 0;
    u16 y0 = 0;
    u16 x1 = 128;
    u16 y1 = 64;
  } window;
};
#endif