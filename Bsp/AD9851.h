#ifndef __AD9851_H
#define __AD9851_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

void AD9851_Init(void);
void AD9851_Write(uint64_t freqword);
uint64_t AD9851_freqword(uint64_t freq);


#ifdef __cplusplus
}
#endif

#endif