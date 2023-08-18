/*
 * @Author       : ZhangRiven
 * @Date         : 2023-06-22 12:32:36
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-06-22 12:56:51
 * @FilePath     : \HAL_F407ZGT6\Application\waveform_generator.hpp
 * @Description  :
 */
#pragma once
#include "task.hpp"
#include "waveform_factory.hpp"

enum class WAVEFORM_TYPE { sine, triangle, square };

/* 波形Strategy接口 */
class WaveFormStrategy {
public:
  virtual double getNextValue() = 0;
};

/* WaveGenerator类 */
class WaveGenerator {
public:
  WaveGenerator(WAVEFORM_TYPE type, uint32_t freq) : type(type), freq(freq) {
    strategy = WaveFormFactory::createWaveForm(type);
  }
  // ...
private:
  WaveFormStrategy *strategy; // 持有一个波形Strategy对象
  WAVEFORM_TYPE type;
  uint32_t freq;
};


/* 正弦波Strategy */
class SineWaveStrategy : public WaveFormStrategy {
public:
  double getNextValue() {
    // ...
  }
};

/* 三角波Strategy */
class TriangleWaveStrategy : public WaveFormStrategy {
public:
  double getNextValue() {
    // ...
  }
};

/* 方波Strategy */
class SquareWaveStrategy : public WaveFormStrategy {
public:
  double getNextValue() {
    // ...
  }
};
