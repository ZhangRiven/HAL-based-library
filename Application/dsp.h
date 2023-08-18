#ifndef __DSP_H
#define  __DSP_H
#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

void PowerMag(uint16_t _usFFTPoints);
void Power_Phase_Radians(uint16_t _usFFTPoints, uint32_t _uiCmpValue);
void DSP_FFTPhase(void);

#ifdef __cplusplus
}
#endif

#endif