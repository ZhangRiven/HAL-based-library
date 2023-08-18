/*
 * @Author       : ZhangRiven
 * @Date         : 2023-07-23 10:30:41
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-07-29 01:14:56
 * @FilePath     : \HAL_F407ZGT6\Bsp\KeyPad.hpp
 * @Description  : 矩阵键盘驱动；行列和为2的倍数时，刚好不浪费申请的内存
 */
#ifndef __KeyPad_H
#define __KeyPad_H

#include "task_app.hpp"
#include "vector.h"

namespace bsp {

template <typename GPIO_Port_t, typename GPIO_Pin_t> class KeyPad {
protected:
  /* 引脚信息结构体 */
  struct key_Pin_t {
    GPIO_Port_t Port;
    GPIO_Pin_t Pin;
  };

private:
  /* 硬件初始化 */
  virtual void hardwareInit() { /* 引脚应全部设置为开漏上拉 */
  }

  /* 写引脚电平，根据底层 继承重写或直接重写 */
  virtual void write_pin(key_Pin_t KEY_Pin, uint8_t PinState) {
    HAL_GPIO_WritePin(reinterpret_cast<GPIO_TypeDef *>(KEY_Pin.Port),
                      reinterpret_cast<uint16_t>(KEY_Pin.Pin),
                      static_cast<GPIO_PinState>(PinState));
  }

  /* 读引脚电平，根据底层 继承重写或直接重写 */
  virtual uint8_t read_pin(key_Pin_t KEY_Pin) {
    return HAL_GPIO_ReadPin(reinterpret_cast<GPIO_TypeDef *>(KEY_Pin.Port),
                            reinterpret_cast<uint16_t>(KEY_Pin.Pin));
  }

public:
  KeyPad() {}
  ~KeyPad() {
    // 释放内存
    for (int i = 0; i < row_key_Pin_vector.size(); i++) {
      delete[] keyStateBuffer[i];
    }
    delete[] keyStateBuffer;
  }

  /* 添加行引脚 */
  KeyPad &add_row_pin(GPIO_Port_t Port, GPIO_Pin_t Pin) {
    row_key_Pin_vector.push_back({Port, Pin});
    return *this;
  }

  /* 添加列引脚 */
  KeyPad &add_column_pin(GPIO_Port_t Port, GPIO_Pin_t Pin) {
    column_key_Pin_vector.push_back({Port, Pin});
    return *this;
  }

  /* 初始化，应在添加引脚后调用 */
  KeyPad &init() {
    if (0 != row_key_Pin_vector.size() && 0 != column_key_Pin_vector.size()) {
      uint8_t row = row_key_Pin_vector.size();
      uint8_t col = column_key_Pin_vector.size();

      // 分配内存
      keyStateBuffer = new uint8_t *[row];
      for (int i = 0; i < row; i++) {
        keyStateBuffer[i] = new uint8_t[col];
      }

      // 内存初始化
      for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
          keyStateBuffer[i][j] = 0;
        }
      }
      hardwareInit();
      write_all_row_pin(1);
      write_all_column_pin(0);
    }
    return *this;
  }

  /* 设置触发函数指针 */
  KeyPad &Set_trig_CallBack(void (*trig_CallBack)(uint32_t)) {
    this->trig_CallBack = trig_CallBack;
    return *this;
  }

  /* 设置去抖时间 */
  KeyPad &Set_debounce(uint8_t debounce) {
    this->shortPressDebounce = debounce;
    return *this;
  }

  /* 读取键值 */
  uint16_t Get_Key_Value() {
    uint16_t key_value_tmp = key_value;
    key_value = 0;
    return key_value_tmp;
  }

  /* 周期调用函数句柄 */
  void Scan_callback_handler() {
    for (uint8_t row = 0; row < row_key_Pin_vector.size(); row++) {
      if (0 == read_pin(row_key_Pin_vector[row])) { // 行低电平为按下
        write_all_row_pin(0);
        write_all_column_pin(1); // 翻转
        for (uint8_t column = 0; column < column_key_Pin_vector.size();
             column++) {
          if (0 == read_pin(column_key_Pin_vector[column])) {
            if (keyStateBuffer[row][column] < 255) { // 防溢出
              keyStateBuffer[row][column]++;         // 低电平计时
            }
            if (keyStateBuffer[row][column] == shortPressDebounce) {
              //>=改为==只触发一次
              key_value = row * 4 + column + 1; // 确定按下，赋键值
              if (nullptr != trig_CallBack) {
                trig_CallBack(key_value); // 调用函数指针
              }
            }
            // } else {
            //   keyStateBuffer[row][column] = 0; // 清空低电平计时
          }
        }
        write_all_row_pin(1);
        write_all_column_pin(0); // 翻转
      } else {                   // 清空低电平计时
        for (uint8_t column = 0; column < column_key_Pin_vector.size();
             column++) {
          keyStateBuffer[row][column] = 0;
        }
      }
    }
  }

  /* 读键值 */
  uint16_t Read_Key_Value() { return key_value; }

private:
  /* 行引脚信息容器 */
  vector<key_Pin_t> row_key_Pin_vector;

  /* 列引脚信息容器 */
  vector<key_Pin_t> column_key_Pin_vector;

  uint16_t key_value; // 键值

  /* 去抖参数 */
  uint8_t shortPressDebounce = 2;
  // uint8_t longPressDebounce = 50;//TODO:增加长按、双击、释放

  /* 键盘触发回调函数 */
  void (*trig_CallBack)(uint32_t key_value) = nullptr;

  /* 写全部列引脚 */
  void write_all_row_pin(uint8_t PinState) {
    for (key_Pin_t key_pin : row_key_Pin_vector) {
      write_pin(key_pin, PinState);
    }
  }

  /* 写全部行引脚 */
  void write_all_column_pin(uint8_t PinState) {
    for (key_Pin_t key_pin : column_key_Pin_vector) {
      write_pin(key_pin, PinState);
    }
  }

  /* 键值暂存数组 */
  uint8_t **keyStateBuffer;
};
}; // namespace bsp
#endif