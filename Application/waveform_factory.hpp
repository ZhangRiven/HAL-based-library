#pragma once
#include "waveform_generator.hpp"
/* 波形Factory */
class WaveFormFactory {
public:
  static WaveFormStrategy *createWaveForm(WAVEFORM_TYPE type) {
    switch (type) {
    case WAVEFORM_TYPE::sine:
      return new SineWaveStrategy();
      break;
    case WAVEFORM_TYPE::triangle:
      return new TriangleWaveStrategy();
      break;
    case WAVEFORM_TYPE::square:
      return new SquareWaveStrategy();
      break;
    }
  }

};