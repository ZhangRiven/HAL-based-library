/*
 * @Author       : ZhangRiven
 * @Date         : 2023-07-30 11:34:02
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-07-30 16:10:21
 * @FilePath     : \HAL_F407ZGT6\Bsp\uart.hpp
 * @Description  :
 */
#pragma once
#include "lib/fifo.hpp"
#include "lib/iofile.hpp"
#include "usart.h"
#include <cstdint>

template <UART_HandleTypeDef *huart, uint32_t FIFO_IN_SIZE = 0,
          uint32_t FIFO_OUT_SIZE = 0>
class Uart_Stream : public lib::IOFile {
public:
  Uart_Stream &enable(bool en = true) {
    uart_handle->Instance->CR1 |= en << 13;
    return *this;
  }

  Uart_Stream &enable_tx(bool en = true) {
    uart_handle->Instance->CR1 |= en << 3;
    return *this;
  }

  Uart_Stream &enable_rx() {
    HAL_UART_Receive_IT(uart_handle, reinterpret_cast<uint8_t *>(&buffer_rx),
                        1);
    return *this;
  }

  int read_char() override {
    if (FIFO_IN_SIZE) {
      char data;
      if (fifo_in.pull(data)) {
        return data;
      }
    } else if (__HAL_UART_GET_FLAG(uart_handle, UART_FLAG_RXNE)) {
      return static_cast<int>(huart->Instance->DR);
    }
    return -1;
  }

  int read_data(char *data, int len) override {
    for (uint32_t n = 0; n < len; n++) {
      data[n] = read_char();
    }
    return 0;
  }

  void write_char(char data) override {
    if (FIFO_OUT_SIZE) {
      while (fifo_out.is_full())
        ;
      __HAL_UART_DISABLE_IT(uart_handle, UART_IT_TXE); // 临界关中断
      if (fifo_out.has_data() ||
          !__HAL_UART_GET_FLAG(uart_handle, UART_IT_TXE)) {
        // fifo有数据||发送缓冲有数据
        fifo_out.push(data);
        __HAL_UART_ENABLE_IT(uart_handle, UART_IT_TXE);
        return;
      }
    } else {
      while (!__HAL_UART_GET_FLAG(uart_handle, UART_FLAG_TXE))
        ;
    }
    tx_data(data);
  }

  void handler() {
    if (__HAL_UART_GET_FLAG(uart_handle, UART_IT_RXNE)) {
      if (FIFO_IN_SIZE) {
        fifo_in.push(buffer_rx);
      }
      enable_rx();
    }
    if (__HAL_UART_GET_FLAG(uart_handle, UART_FLAG_TXE)) {
      if (FIFO_OUT_SIZE) {
        char data;
        if (fifo_out.pull(data)) {
          tx_data(data);
        }
        if (fifo_out.is_empty()) {
          __HAL_UART_DISABLE_IT(uart_handle, UART_IT_TXE);
        }
      } else {
        __HAL_UART_DISABLE_IT(uart_handle, UART_IT_TXE);
      }
    }
  }

private:
  UART_HandleTypeDef *uart_handle = huart;
  lib::Fifo<char, FIFO_IN_SIZE> fifo_in;
  lib::Fifo<char, FIFO_OUT_SIZE> fifo_out;
  char buffer_rx;

  void tx_data(uint32_t data) { uart_handle->Instance->DR = data; }
};