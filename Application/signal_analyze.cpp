/*
 * @Author       : ZhangRiven
 * @Date         : 2023-08-03 10:45:46
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-06 09:53:32
 * @FilePath     : \HAL_F407ZGT6\Application\signal_analyze.cpp
 * @Description  :
 */
#include "signal_analyze.h"
#include "task_screen.h"
#include <stdio.h>
float threshold = 0.02;
const char *modulation_type;     // 调制类型
char param1[32];                 // 参数打印用缓存
float freq = 0;                  // 波形频率
uint16_t over_threshold_num = 0; // 超过阈值点数
uint16_t main_lobe_index = 0;    // 主瓣中心
float freq_cacluate(uint32_t i) { return (float)i * fs / fftlen; }

void cacluate_AM_Ma() {
  modulation_type = "AM";
  float Ma = (float)Output_f32[main_lobe_index] / Output_f32[0] +
             0.034; // 计算调制深度
  sprintf(param1, "Ma:%.3f", Ma);
}


void cacluate_FM_df_Mf() {
  modulation_type = "FM";
  float df = 0;
  adc_fm_all_in(&df);
  float Mf = df / (2 * freq) - 1;
  sprintf(param1, "mf:%.2f,dfmax:%.2fkHz", Mf, df);
}

void cacluate_CW() {
  modulation_type = "CW";
  sprintf(param1, " ");
}

void cacluate_2ASK() {
  modulation_type = "2ASK";
  sprintf(param1, "Rc:%.1fkbps", freq * 2);
}

void cacluate_2FSK_Rc_h() {
  modulation_type = "2FSK";
  float Rc = freq * 2;//二进制码速率
  float h = (main_lobe_index * (fs2 / (float)fftlen)) / Rc/4;
  //键控系数
  sprintf(param1, "Rc:%.1fkbps,h:%.1f", Rc, h);
}

void cacluate_2PSK() {
  modulation_type = "2PSK";
  sprintf(param1, "Rc:%.1fkbps", freq * 2);
}

void No_sign() {
  modulation_type = nullptr;
  sprintf(param1, " ");
}


/* 找主瓣、超阈值计数 */
uint16_t find_main_lobe(float32_t *mag, uint16_t len, uint16_t *over_thr_num,
                        uint16_t *main_lobe_i) {
  *over_thr_num = 0;                       // 复位超过阈值的点数
  *main_lobe_i = 0;                        // 复位主瓣中心值
  float max_value = 0;                     // 最大值
  for (uint16_t i = 1; i < len / 2; i++) { // 从1开始跳过0Hz
    if (mag[i] > threshold) {              // 幅值超过阈值
      (*over_thr_num)++;                   // 超过阈值的点数+1
      if (mag[i] > max_value) {
        *main_lobe_i = i; // 主瓣中心
        max_value = mag[i];
      }
    }
  }
  return *main_lobe_i;
}

/* 方波返回1 */
uint16_t check_Square(uint16_t Max_mag_index) {
  uint16_t over_threshold_num = 0;
  for (uint16_t index = Max_mag_index; index < Max_mag_index * 5 + 1; index++) {
    if (Output_f32[index] >= Output_f32[Max_mag_index * 3]) {
      over_threshold_num++;
    }
  }
  if (over_threshold_num == 2) { // 一三次谐波
    return 1;
  } else {
    return 0;
  }
}

/* FM失真会有123次谐波，识别到返回1 */
uint16_t check_FM(uint16_t Max_mag_index) {
  uint16_t over_threshold_num = 0;
  for (uint16_t index = Max_mag_index; index < Max_mag_index * 5 + 1; index++) {
    if (Output_f32[index] >= Output_f32[Max_mag_index * 3]) {
      over_threshold_num++;
    }
  }
  if (over_threshold_num == 3) { // 一二三次谐波
    return 1;
  } else {
    return 0;
  }
}
uint16_t check_wave(uint8_t ch) { // 识别到返回1
  find_main_lobe(Output_f32, fftlen, &over_threshold_num, &main_lobe_index);
  freq = freq_cacluate(main_lobe_index); // 计算频率
  if (freq < 0.8) {                      // 频率小于0.8k，无信号
    return 0;
  }
  if (over_threshold_num == 1) { // 只有一个柱子就是sine
    return 1;
  } else if (over_threshold_num > 1) {
    if (check_Square(main_lobe_index)) {
      return 2; // square
    } else if ((ch==1)&&check_FM(main_lobe_index)){//对FM失真做的特殊处理
			return 1;
		}		
  }
  return 0;
}

/* dfmax,应在找主瓣后调用 */
float max_freq_dev(uint16_t *main_lobe_i, float *mag_f32, uint16_t len,
                   float fs) {
  // 查找频谱最大值
  float max_val = mag_f32[*main_lobe_i];
  uint16_t max_index = *main_lobe_i;

  // 从最大值频点向两边查找半功点
  int f1 = max_index - 100;
  int f2 = max_index + 100;
  float half_power = max_val * 0.3;
  while (mag_f32[f1] < half_power)
    f1++;
  while (mag_f32[f2] < half_power)
    f2--;

  // 计算两半功点间频差
  float dfmax = ((f2 - f1) * fs / len / 2.0);
  return dfmax;
}

/* 时域求最大频偏,作废 */
// 输入:
// x - 时域采样数据
// fs - 采样率
// N - 数据长度
float maxFreqDeviation(uint16_t *x, float fs, int N) {
  float phi = 0;
  float d_phi, d_phi_max = 0;
  for (int n = 1; n < N; n++) {
    // 计算相位差并内联
    d_phi = (x[n] - x[n - 1]) * 2 * PI / UINT16_MAX;
    phi += d_phi;
    if (d_phi > d_phi_max) {
      d_phi_max = d_phi;
    }
  }
  return d_phi_max * fs / (2 * PI);
}

uint8_t signal_type_screen_print() {
  uint8_t have_sign = 0;
  char str_buffer[16]; // 调制类型
  if (NULL != modulation_type) {
    have_sign = 1;
    sprintf(str_buffer, "type:%s", modulation_type);
    screen.Show_string(10, 0, 160, 32, str_buffer, LCD::FONT_TYPE_SIZE::_32,
                       LCD::COLOR_16BIT::RED, 1, LCD::COLOR_16BIT::BLACK);

    /* 显示频率 */
    sprintf(str_buffer, "Freq:%.2fkHz", freq);
    screen.Show_string(10, 32, 239, 32, str_buffer, LCD::FONT_TYPE_SIZE::_24,
                       LCD::COLOR_16BIT::YELLOW, 1, LCD::COLOR_16BIT::BLACK);

    /* 显示调制参数 */
    screen
        .Show_string(10, 56, 300, 32, param1, LCD::FONT_TYPE_SIZE::_24,
                     LCD::COLOR_16BIT::BLUE, 1, LCD::COLOR_16BIT::BLACK)
        .Show_string(10, 90, 300, 16, "Press 1 start next scan.",
                     LCD::FONT_TYPE_SIZE::_16, LCD::COLOR_16BIT::WHITE, 1,
                     LCD::COLOR_16BIT::BLACK);

  } else {
    /* 清屏 */
    screen.Fill(150, 56, 89, 64, LCD::COLOR_16BIT::BLACK)
        .Show_string(10, 0, 200, 32 * 3, "type:CW", LCD::FONT_TYPE_SIZE::_32,
                     LCD::COLOR_16BIT::RED, 1, LCD::COLOR_16BIT::BLACK);
  }
  /* 打印主瓣 */
  sprintf(str_buffer, "%d,%d,%.2f", over_threshold_num, main_lobe_index,
          Output_f32[main_lobe_index]);
  screen.Show_string(200, 220, 120, 16, str_buffer, LCD::FONT_TYPE_SIZE::_16,
                     LCD::COLOR_16BIT::BLUE, 1, LCD::COLOR_16BIT::BLACK);
  return have_sign;
}