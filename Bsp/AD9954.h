/*
 * @Author       : ZhangRiven
 * @Date         : 2016-12-20 09:31:09
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-05 09:45:00
 * @FilePath     : \HAL_F407ZGT6\Bsp\AD9954.h
 * @Description  :
 */
#ifndef __AD9954_H
#define __AD9954_H
#include "task_app.h"

#ifdef __cplusplus
extern "C" {
#endif
/*

**  AD9954_CS----------PB9  OUT
**  AD9954_SCLK--------PB10 OUT
**  AD9954_SDIO--------PB11 OUT
**  AD9954_OSK---------PB12 OUT
**  PS0----------------PB13 OUT
**  PS1----------------PB14 OUT
**  IOUPDATE-----------PB15 OUT

**  AD9954_SDO---------PB5  IN
**  AD9954_IOSY--------PB6  OUT
**  AD9954_RES---------PB7  OUT
**  AD9954_PWR---------PB8  OUT
*/

// #define AD9954_CS PAout(3)
// #define AD9954_SCLK PAout(4)
// #define AD9954_SDIO PAout(5)
// #define AD9954_OSK PCout(0)
// #define PS0 PAout(2)
// #define PS1 PBout(10)
// #define IOUPDATE PAout(7)

// #define AD9954_SDO PAin(8)
// #define AD9954_IOSY PBout(1)
// #define AD9954_RES PAout(6)
// #define AD9954_PWR PBout(0)

#define AD9954_CS(x) HAL_GPIO_WritePin(AD9954_CS_GPIO_Port, AD9954_CS_Pin, x)
#define AD9954_SCLK(x)                                                         \
  HAL_GPIO_WritePin(AD9954_SCLK_GPIO_Port, AD9954_SCLK_Pin, x)
#define AD9954_SDIO(x)                                                         \
  HAL_GPIO_WritePin(AD9954_SDIO_GPIO_Port, AD9954_SDIO_Pin, x)
#define AD9954_OSK(x) HAL_GPIO_WritePin(AD9954_OSK_GPIO_Port, AD9954_OSK_Pin, x)
#define PS0(x) HAL_GPIO_WritePin(AD9954_PS0_GPIO_Port, AD9954_PS0_Pin, x)
#define PS1(x) HAL_GPIO_WritePin(AD9954_PS1_GPIO_Port, AD9954_PS1_Pin, x)
#define IOUPDATE(x)                                                            \
  HAL_GPIO_WritePin(AD9954_IOUPDATE_GPIO_Port, AD9954_IOUPDATE_Pin, x)

#define AD9954_SDO(x) HAL_GPIO_ReadPin(AD9954_SDO_GPIO_Port, AD9954_SDO_Pin)
#define AD9954_IOSY(x)                                                         \
  HAL_GPIO_WritePin(AD9954_IOSY_GPIO_Port, AD9954_IOSY_Pin, x)
#define AD9954_RES(x) HAL_GPIO_WritePin(AD9954_RES_GPIO_Port, AD9954_RES_Pin, x)
#define AD9954_PWR(x) HAL_GPIO_WritePin(AD9954_PWR_GPIO_Port, AD9954_PWR_Pin, x)

#define CFR1 0X00 // 控制功能寄存器1
#define CFR2 0X01 // 控制功能寄存器2
#define ASF 0X02  // 振幅比例因子寄存器
#define ARR 0X03  // 振幅斜坡速率寄存器

#define FTW0 0X04 // 频率调谐字寄存器0
#define POW0 0X05 // 相位偏移字寄存器
#define FTW1 0X06 // 频率调谐字寄存器1

#define NLSCW 0X07 // 下降扫描控制字寄存器
#define PLSCW 0X08 // 上升扫描控制字寄存器

#define RSCW0 0X07 //
#define RSCW1 0X08 //
#define RSCW2 0X09 //
#define RSCW3 0X0A // RAM段控制字寄存器
#define RAM 0X0B   // 读取指令写入RAM签名寄存器数据

#define No_Dwell 0x04 // No_Dwell不停留，输出扫频到终止频率回到起始频率。
#define Dwell 0x00 // Dwell停留，输出扫频到终止频率后保持在终止频率。

void AD9954_GPIO_Init(void); // 初始化控制AD9954需要用到的IO口
void AD9954_RESET(void);     // 复位AD9954
void UPDATE(void); // 产生一个更新信号，更新AD9954内部寄存器

void AD9954_Send_Byte(uint8_t dat); // 向AD9954发送一个字节的内容
uint8_t AD9954_Read_Byte(void);     // 读AD9954一个字节的内容
void AD9954_Write_nByte(uint8_t RegAddr, uint8_t *Data,
                        uint8_t Len); // 向AD9954指定的寄存器写数据
uint32_t AD9954_Read_nByte(uint8_t ReadAddr, uint8_t Len); // 读AD9954寄存器数据
uint32_t Get_FTW(double Real_fH);                          // 频率数据转换

void AD9954_Init(void); // 初始化AD9954的管脚和内部寄存器的配置，
void AD9954_Set_Fre(double fre);       // 设置AD9954输出频率，点频
void AD9954_Set_Amp(uint16_t Ampli);   // 写幅度
void AD9954_Set_Phase(uint16_t Phase); // 写相位

void AD9954_SetFSK(double f1, double f2, double f3, double f4,
                   uint16_t Ampli); // AD9954的FSK参数设置
void AD9954_SetPSK(uint16_t Phase1, uint16_t Phase2, uint16_t Phase3,
                   uint16_t Phase4, double fre,
                   uint16_t Ampli); // AD9954的PSK参数设置
void AD9954_Set_LinearSweep(double Freq_Low, double Freq_High,
                            double UpStepFreq, uint8_t UpStepTime,
                            double DownStepFreq, uint8_t DownStepTime,
                            uint8_t mode); // 扫频参数设置

#ifdef __cplusplus
}
#endif
#endif
