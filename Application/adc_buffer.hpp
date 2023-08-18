/*
 * @Author       : ZhangRiven
 * @Date         : 2023-06-19 21:06:16
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-04 12:56:10
 * @FilePath     : \HAL_F407ZGT6\Application\adc_buffer.hpp
 * @Description  :
 */
#ifndef __ADC_BUFFER_H__
#define __ADC_BUFFER_H__
#include "task_app.hpp"
#include <cstdint>

/**
 * @description: 获取字符串长度
 * @param {char} *string
 * @return {*}
 */
static u8 get_string_length(char *string) {
  u8 length = 0;
  while ('\0' != *string) {
    length++;
    string++;
  }
  return length;
}

template <typename TYPE>

class adc_buffer {
public:
  /**
   * @description: 构造函数
   * @param {u32} Sampling_number 采样点数
   * @return {*}
   */
  adc_buffer(u32 Sampling_number = 128) : Sampling_number(Sampling_number) {
    time_tick = 0;
    buffer = nullptr;
    finished = 0;
  }

  ~adc_buffer() { delete[] buffer; }

  /**
   * @description: 获取采样点数
   * @return {*}
   */
  u32 Get_Sampling_number() {
    u32 n = Sampling_number;
    return n;
  }

  /**
   * @description: 初始化采样存储缓存
   * @return {*}
   */
  adc_buffer &Init_Buffer() {
    if (nullptr == buffer)
      buffer = new TYPE[Sampling_number];
    return *this;
  }

  /**
   * @description: 获取数据数组指针
   * @return {*}
   */
  TYPE *Get_buffer_ptr() {
    if (nullptr != buffer) {
      return buffer;
    }
    return nullptr;
  }

  /**
   * @description: 获取数据，从0开始
   * @param {u32} n
   * @return {*}
   */
  TYPE Get_data(u32 n) {
    if (nullptr != buffer) {
      if (0 <= n && n < Sampling_number)
        return buffer[n];
    }
    return -1;
  }
  /**
   * @description: 计时的tick回调函数
   * @return {*}
   */
  adc_buffer &Time_tick_Callback() {
    time_tick++;
    return *this;
  }

  /**
   * @description: 获取计时
   * @return {*}
   */
  u32 Get_time_tick() { return time_tick; } // TODO:未完成，增加计算时间

  /**
   * @description: 清除计时
   * @return {*}
   */
  adc_buffer &Clear_time_tick() {
    time_tick = 0;
    return *this;
  }

  /**
   * @description: 设置完成标志位
   * @param {u8} finished
   * @return {*}
   */
  adc_buffer &Set_finished_flag() {
    this->finished = 1;
    return *this;
  }

  /**
   * @description: 获取完成标志
   * @return {*}
   */
  u8 Get_finished_flag() {
    return this->finished;
  }
  
  adc_buffer &Reset_finished_flag() {
    this->finished = 0;
    return *this;
  }

private:
  const u32 Sampling_number; // 采样次数
  u32 time_tick;             // 花费时间
  TYPE *buffer;              // 采样存储缓存
  volatile u8 finished;           // 完成标志
};

#endif