#ifndef __SIGNAL_ANALYZE_H
#define __SIGNAL_ANALYZE_H
#include "fft_analyze.hpp"

#ifdef __cplusplus
extern "C" {
#endif

extern uint16_t main_lobe_index;// 存储超过阈值的点
extern uint16_t over_threshold_num;//超过阈值点数
void cacluate_AM_Ma();
void cacluate_2ASK();
void cacluate_FM_df_Mf();
void cacluate_2FSK_Rc_h();
void cacluate_CW();
void cacluate_2PSK();
void No_sign();
uint16_t check_wave(uint8_t ch);
uint8_t signal_type_screen_print();
uint16_t find_main_lobe(float32_t *mag, uint16_t len, uint16_t *over_thr_num,
                        uint16_t *main_lobe_i);
float max_freq_dev(uint16_t *main_lobe_i,float *mag_f32,uint16_t len, float fs);
float maxFreqDeviation(uint16_t *x, float fs, int N);
#ifdef __cplusplus
}
#endif

#endif