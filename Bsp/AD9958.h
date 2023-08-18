/*
 * @Author       : ZhangRiven
 * @Date         : 2023-07-29 00:19:08
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-07-29 13:03:29
 * @FilePath     : \HAL_F407ZGT6\Bsp\ad9958.h
 * @Description  :
 */
#ifndef _AD9958_H_
#define _AD9958_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

// AD9959寄存器地址定义
#define CSR_ADD 0x00   // CSR 通道选择寄存器
#define FR1_ADD 0x01   // FR1 功能寄存器1
#define FR2_ADD 0x02   // FR2 功能寄存器2
#define CFR_ADD 0x03   // CFR 通道功能寄存器
#define CFTW0_ADD 0x04 // CTW0 通道频率转换字寄存器
#define CPOW0_ADD 0x05 // CPW0 通道相位转换字寄存器
#define ACR_ADD 0x06   // ACR 幅度控制寄存器
#define LSRR_ADD 0x07  // LSR 通道线性扫描寄存器
#define RDW_ADD 0x08   // RDW 通道线性向上扫描寄存器
#define FDW_ADD 0x09   // FDW 通道线性向下扫描寄存器

typedef enum {
  AD9958_CHANNEL_0 = 0x40,
  AD9958_CHANNEL_1 = 0x80,
} AD9958_Channel_TypeDef;

typedef enum {
  AD9958_DISABLE = 0X00,
  AD9958_AMPLITUDE = 0X01,
  AD9958_FREQUENCY = 0X02,
  AD9958_PHASE = 0X03,
} AD9958_CFRMOOD_TypeDef;

typedef enum {
  LINEAR_CLOSE = 0,
  LINEAR_OPEN = 1,
} AD9958_LinearSweep_TypeDef;


void Init_AD9959(void); // AD9958~AD9959初始化
void Write_frequence(uint32_t Freq);         // 设置通道输出频率
void Write_Amplitude(uint16_t Ampli);        // 设置通道输出幅度
void Write_Phase(uint16_t Phase);            // 设置通道输出相位
void LINEAR_SWEEP_MODE(AD9958_Channel_TypeDef Channel, // 通道选择
                       float Rising,          // 上升时间隔时间(纳秒)
                       float Falling,         // 下降时间隔时间(纳秒)
                       float RisingFrequency, // 设置上升扫描每次增加的频率
                       float FallingFrequency, // 设置下降扫描每次减少的频率
                       uint32_t Freq,          // 设置通道输出频率
                       float Data              // 设置扫描终止频率
);


void SDIO0_MOODPUT(uint16_t pdata); // SDIO0的IO口模式转换
void Intserve(void);                // IO口初始化
void IntReset(void);                // AD9959复位
void IO_Update(void);               // AD9959更新数据
void WriteData_AD9959(uint8_t RegisterAddress, uint8_t NumberofRegisters,
                      uint8_t *RegisterData, uint8_t temp); // 写入数据
void ReadData_AD9959(uint8_t RegisterAddress, uint8_t NumberofRegisters,
                     uint8_t *RegisterData, uint8_t temp); // 读取数据
void Write_ChoiceChannel(AD9958_Channel_TypeDef Channel);  // 通道选择
void Write_SetPLLDividerRatio(uint8_t Data); // 设置主频的倍频系数
void Write_SetBlankingTimeNs(float Rising, float Falling); // 设置扫描间隔时间
void Write_SetFrequencyRisingLSR(float Frequency); // 设置上升扫描每次增加的频率
void Write_SetFrequencyFallingLSR(
    float Frequency); // 设置下降扫描每次减少的频率
void Write_SetChannelWord(float Data, AD9958_CFRMOOD_TypeDef Mode,
                          uint8_t CW_number);       // 设置扫描终止频率
void Write_ModulationLevelSelection(uint8_t Level); // 设置调制进制
void Write_ModulationTypeConfiguration(
    AD9958_CFRMOOD_TypeDef Mode,
    AD9958_LinearSweep_TypeDef NewState); // 设置调制模式
#ifdef __cplusplus
}
#endif
#endif
