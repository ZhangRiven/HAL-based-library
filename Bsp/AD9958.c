#include "AD9958.h"

#include "task_app.h"

// AD9959管脚宏定义
#define PDC_Pin GPIO_PIN_0
#define PDC_GPIO_Port GPIOB
// #define D2_Pin GPIO_PIN_4
// #define D2_GPIO_Port GPIOC
#define D0_Pin GPIO_PIN_6
#define D0_GPIO_Port GPIOA
#define SCLK_Pin GPIO_PIN_4
#define SCLK_GPIO_Port GPIOA
#define UPDATE_Pin GPIO_PIN_2
#define UPDATE_GPIO_Port GPIOA
// #define P2_Pin GPIO_PIN_0
// #define P2_GPIO_Port GPIOA
// #define P0_Pin GPIO_PIN_2
// #define P0_GPIO_Port GPIOC

#define G_Pin GPIO_PIN_5
#define G_GPIO_Port GPIOC
// #define D3_Pin GPIO_PIN_7
// #define D3_GPIO_Port GPIOA
// #define D1_Pin GPIO_PIN_5
// #define D1_GPIO_Port GPIOA
#define REST_Pin GPIO_PIN_3
#define REST_GPIO_Port GPIOA
#define CS_Pin GPIO_PIN_1
#define CS_GPIO_Port GPIOA
// #define P3_Pin GPIO_PIN_3
// #define P3_GPIO_Port GPIOC
// #define P1_Pin GPIO_PIN_1
// #define P1_GPIO_Port GPIOC

inline static void GPIO_SetBits(GPIO_TypeDef *port, uint16_t pin) {
  HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
}
inline static void GPIO_ResetBits(GPIO_TypeDef *port, uint16_t pin) {
  HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
}
inline static uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef *port, uint16_t pin) {
  if (HAL_GPIO_ReadPin(port, pin)) {
    return 1;
  } else {
    return 0;
  }
}
// CSB
#define CS_1() GPIO_SetBits(CS_GPIO_Port, CS_Pin)
#define CS_0() GPIO_ResetBits(CS_GPIO_Port, CS_Pin)
// SCK
#define SCLK_1() GPIO_SetBits(SCLK_GPIO_Port, SCLK_Pin)
#define SCLK_0() GPIO_ResetBits(SCLK_GPIO_Port, SCLK_Pin)
// UPD
#define UPDATE_1() GPIO_SetBits(UPDATE_GPIO_Port, UPDATE_Pin)
#define UPDATE_0() GPIO_ResetBits(UPDATE_GPIO_Port, UPDATE_Pin)
// P0
// #define PS0_1() GPIO_SetBits(P0_GPIO_Port, P0_Pin)
// #define PS0_0() GPIO_ResetBits(P0_GPIO_Port, P0_Pin)
// P1
// #define PS1_1() GPIO_SetBits(P1_GPIO_Port, P1_Pin)
// #define PS1_0() GPIO_ResetBits(P1_GPIO_Port, P1_Pin)
// P2
// #define PS2_1() GPIO_SetBits(P2_GPIO_Port, P2_Pin)
// #define PS2_0() GPIO_ResetBits(P2_GPIO_Port, P2_Pin)
// P3
// #define PS3_1() GPIO_SetBits(P3_GPIO_Port, P3_Pin)
// #define PS3_0() GPIO_ResetBits(P3_GPIO_Port, P3_Pin)
// IO0
#define SDIO0_1() GPIO_SetBits(D0_GPIO_Port, D0_Pin)
#define SDIO0_0() GPIO_ResetBits(D0_GPIO_Port, D0_Pin)
// IO1
// #define SDIO1_1() GPIO_SetBits(D1_GPIO_Port, D1_Pin)
// #define SDIO1_0() GPIO_ResetBits(D1_GPIO_Port, D1_Pin)
// IO2
// #define SDIO2_1() GPIO_SetBits(D2_GPIO_Port, D2_Pin)
// #define SDIO2_0() GPIO_ResetBits(D2_GPIO_Port, D2_Pin)
// IO3
// #define SDIO3_1() GPIO_SetBits(D3_GPIO_Port, D3_Pin)
// #define SDIO3_0() GPIO_ResetBits(D3_GPIO_Port, D3_Pin)
// PDC
// #define AD9959_PWR_1() GPIO_SetBits(PDC_GPIO_Port, PDC_Pin)
// #define AD9959_PWR_0() GPIO_ResetBits(PDC_GPIO_Port, PDC_Pin)
// RST
#define Reset_1() GPIO_SetBits(REST_GPIO_Port, REST_Pin)
#define Reset_0() GPIO_ResetBits(REST_GPIO_Port, REST_Pin)
// Read_GPIO_D0_pin
#define SDIO0_ReadBit() GPIO_ReadInputDataBit(D0_GPIO_Port, D0_Pin)
// 延时
void delay1(uint32_t length) {
  length = length * 12;
  while (length--)
    ;
}

// AD9959初始化
void Init_AD9959(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  HAL_GPIO_WritePin(G_GPIO_Port, G_Pin, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                        GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  GPIO_InitStruct.Pin =
      GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  Intserve(); // IO口初始化
  IntReset(); // AD9959复位

  Write_SetPLLDividerRatio(20);
}

// SDIO0输入输出模式切换
void SDIO0_MOOD_PUT(uint16_t pdata) {
  HAL_GPIO_WritePin(D0_GPIO_Port, D0_Pin, GPIO_PIN_SET);
  // GPIO_InitTypeDef GPIO_InitStructure;
  // if (pdata) {
  //   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   // 输出模式
  //   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  // 推挽输出
  //   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;    // 上拉
  //   GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed; // 速度选择
  //   GPIO_InitStructure.GPIO_Pin = D0_Pin;           // 初始化 INT 引脚
  //   GPIO_Init(D0_GPIO_Port, &GPIO_InitStructure);
  // } else {
  //   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;     // 输出模式
  //   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // 浮空
  //   GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed;  // 速度选择
  //   GPIO_InitStructure.GPIO_Pin = D0_Pin;            // 初始化 INT 引脚
  //   GPIO_Init(D0_GPIO_Port, &GPIO_InitStructure);
  // }
}
// IO口初始化
void Intserve(void) {
  // AD9959_PWR_0();
  CS_1();
  SCLK_0();
  UPDATE_0();
  // PS0_0();
  // PS1_0();
  // PS2_0();
  // PS3_0();
  SDIO0_0();
  // SDIO1_0();
  // SDIO2_0();
  // SDIO3_0();
}
// AD9959复位
void IntReset(void) {
  Reset_0();
  delay1(1);
  Reset_1();
  delay1(30);
  Reset_0();
  delay1(30);
}
// AD9959更新数据
void IO_Update(void) {
  UPDATE_0();
  delay1(2);
  UPDATE_1();
  delay1(4);
  UPDATE_0();
}
/*--------------------------------------------
函数功能：控制器通过SPI向AD9959写数据
RegisterAddress: 寄存器地址
NumberofRegisters: 所含字节数
*RegisterData: 数据起始地址
temp: 是否更新IO寄存器
----------------------------------------------*/
void WriteData_AD9959(uint8_t RegisterAddress, uint8_t NumberofRegisters,
                      uint8_t *RegisterData, uint8_t temp) {
  uint8_t ControlValue = 0;
  uint8_t ValueToWrite = 0;
  uint8_t RegisterIndex = 0;
  uint8_t i = 0;

  ControlValue = RegisterAddress;
  // 写入地址
  CS_1();
  SCLK_0();
  CS_0();
  for (i = 0; i < 8; i++) {
    SCLK_0();
    if (0x80 & (ControlValue & 0x80)) {
      SDIO0_1();
    } else {
      SDIO0_0();
    }
    SCLK_1();
    ControlValue <<= 1;
  }
  SCLK_0();
  delay1(1);
  // 写入数据
  for (RegisterIndex = 0; RegisterIndex < NumberofRegisters; RegisterIndex++) {
    ValueToWrite = RegisterData[RegisterIndex];
    for (i = 0; i < 8; i++) {
      SCLK_0();
      if (0x80 & (ValueToWrite & 0x80)) {
        SDIO0_1();
      } else {
        SDIO0_0();
      }
      SCLK_1();
      ValueToWrite <<= 1;
    }
    SCLK_0();
  }
  SCLK_0();
  if (temp) {
    IO_Update();
  }
  CS_1();
}
/*--------------------------------------------
函数功能：控制器通过SPI向AD9959读数据
RegisterAddress: 寄存器地址
NumberofRegisters: 所含字节数
*RegisterData: 数据起始地址
temp: 是否更新IO寄存器
----------------------------------------------*/
void ReadData_AD9959(uint8_t RegisterAddress, uint8_t NumberofRegisters,
                     uint8_t *RegisterData, uint8_t temp) {
  uint8_t ControlValue = 0;
  uint8_t RegisterIndex = 0;
  uint8_t ReceiveData = 0;
  uint8_t i = 0;

  ControlValue = RegisterAddress | 0x80;
  // 写入地址
  CS_1();
  SCLK_1();
  CS_0();
  for (i = 0; i < 8; i++) {
    SCLK_0();
    if (0x80 & (ControlValue & 0x80)) {
      SDIO0_1();
    } else {
      SDIO0_0();
    }
    SCLK_1();
    ControlValue <<= 1;
  }
  delay1(1);
  SDIO0_MOOD_PUT(0);
  // 读出数据
  for (RegisterIndex = 0; RegisterIndex < NumberofRegisters; RegisterIndex++) {

    for (i = 0; i < 8; i++) {
      SCLK_0();
      if (SDIO0_ReadBit()) {
        ReceiveData |= 1;
      }
      SCLK_1();
      ReceiveData <<= 1;
    }
    RegisterData[RegisterIndex] = ReceiveData;
  }
  CS_1();
  SDIO0_MOOD_PUT(1);
  delay1(1);
}
/*---------------------------------------
函数功能：设置要操作的通道
Channel:  输出通道
---------------------------------------*/
void Write_ChoiceChannel(AD9958_Channel_TypeDef Channel) {
  uint8_t CSR_DATA[1] = {0x00};

  CSR_DATA[0] = Channel & 0xF0;

  WriteData_AD9959(CSR_ADD, 1, CSR_DATA, 0);
}
/*---------------------------------------
函数功能：设置主频的倍频系数
Data:  倍频系数(4 ~~~ 20)
---------------------------------------*/
void Write_SetPLLDividerRatio(uint8_t Data) {
  uint8_t FR1_DATA[3] = {
      0x00, 0x00, 0x00}; // 20倍频 Charge pump control = 75uA FR1<23> -- VCO
                         // gain control =0时 system clock below 160 MHz;

  Data %= 64;

  FR1_DATA[0] = ((uint8_t)Data << 2) | 0x80;
  FR1_DATA[1] = 0x50;

  WriteData_AD9959(FR1_ADD, 3, FR1_DATA, 0);
}
/*---------------------------------------
函数功能：设置通道输出频率
Freq:     输出频率
---------------------------------------*/
void Write_frequence(uint32_t Freq) {
  uint8_t CFTW0_DATA[4] = {0x00, 0x00, 0x00, 0x00}; // 中间变量
  uint32_t Temp;
  Temp =
      (uint32_t)Freq *
      8.589934592; // 将输入频率因子分为四个字节  4.294967296=(2^32)/500000000

  CFTW0_DATA[3] = (uint8_t)Temp;
  CFTW0_DATA[2] = (uint8_t)(Temp >> 8);
  CFTW0_DATA[1] = (uint8_t)(Temp >> 16);
  CFTW0_DATA[0] = (uint8_t)(Temp >> 24);

  WriteData_AD9959(CFTW0_ADD, 4, CFTW0_DATA,
                   1); // CTW0 address 0x04.输出CH0设定频率
}
/*---------------------------------------
函数功能：设置通道输出幅度
Ampli:    输出幅度(0~1023)
---------------------------------------*/
void Write_Amplitude(uint16_t Ampli) {
  uint16_t A_temp; //=0x23ff;
  uint8_t ACR_DATA[3] = {0x00, 0x00,
                         0x00}; // default Value = 0x--0000 Rest = 18.91/Iout

  A_temp = Ampli | 0x1000;

  ACR_DATA[2] = (uint8_t)A_temp;        // 低位数据
  ACR_DATA[1] = (uint8_t)(A_temp >> 8); // 高位数据

  WriteData_AD9959(ACR_ADD, 3, ACR_DATA, 1);
}
/*---------------------------------------
函数功能：设置通道输出相位
Phase:    输出相位,范围：0~360(对应角度：0°~360°)
---------------------------------------*/
void Write_Phase(uint16_t Phase) {
  uint16_t P_temp = 0;
  uint8_t CPOW0_DATA[2] = {0x00,
                           0x00}; // default Value = 0x0000   @ = POW/2^14*360

  P_temp = (uint16_t)Phase / 360.0f * 16384.0f;

  CPOW0_DATA[1] = (uint8_t)P_temp;
  CPOW0_DATA[0] = (uint8_t)(P_temp >> 8);

  WriteData_AD9959(CPOW0_ADD, 2, CPOW0_DATA, 1);
}
/*---------------------------------------
函数功能：设置扫描间隔时间
Rising:   上升时间隔时间(纳秒)
Falling:  下降时间隔时间(纳秒)
---------------------------------------*/
void Write_SetBlankingTimeNs(float Rising, float Falling) {
  uint8_t LSRR_DATA[2] = {0x00, 0x00}; // default Value = 0x----

  Falling *= (500000000.0f / 4000000000.0f);
  Rising *= (500000000.0f / 4000000000.0f);

  if (Falling < 1) {
    Falling = 1;
  } else if (Falling > 255) {
    Falling = 255;
  }

  if (Rising < 1) {
    Rising = 1;
  } else if (Rising > 255) {
    Rising = 255;
  }

  LSRR_DATA[0] = Falling;
  LSRR_DATA[1] = Rising;

  WriteData_AD9959(LSRR_ADD, 2, LSRR_DATA, 1);
}
/*---------------------------------------
函数功能：设置上升扫描每次增加的频率
Frequency:  频率值(0.116416 ~~~ 200000000.0)
---------------------------------------*/
void Write_SetFrequencyRisingLSR(float Frequency) {
  uint32_t frequency_tuning_word;
  uint8_t RDW_DATA[4] = {0x00, 0x00, 0x00, 0x00}; // default Value = 0x--------

  /* frequency_tuning_word = Frequency * 2^32 / (SystemClock * PLLDividerRatio)
   */
  frequency_tuning_word = (uint32_t)Frequency * 8.589934592;

  RDW_DATA[0] = frequency_tuning_word >> 24;
  RDW_DATA[1] = frequency_tuning_word >> 16;
  RDW_DATA[2] = frequency_tuning_word >> 8;
  RDW_DATA[3] = frequency_tuning_word >> 0;

  WriteData_AD9959(RDW_ADD, 4, RDW_DATA, 1);
}
/*---------------------------------------
函数功能：设置下降扫描每次减少的频率
Frequency:  频率值(0.116416 ~~~ 200000000.0)
---------------------------------------*/
void Write_SetFrequencyFallingLSR(float Frequency) {
  uint32_t frequency_tuning_word;
  uint8_t FDW_DATA[4] = {0x00, 0x00, 0x00, 0x00}; // default Value = 0x--------

  /* frequency_tuning_word = Frequency * 2^32 / (SystemClock * PLLDividerRatio)
   */
  frequency_tuning_word = (uint32_t)Frequency * 8.589934592;

  FDW_DATA[0] = frequency_tuning_word >> 24;
  FDW_DATA[1] = frequency_tuning_word >> 16;
  FDW_DATA[2] = frequency_tuning_word >> 8;
  FDW_DATA[3] = frequency_tuning_word >> 0;

  WriteData_AD9959(FDW_ADD, 4, FDW_DATA, 1);
}
/*---------------------------------------
函数功能：设置扫描终止频率
Data: 数据
Mode: 调制模式
CW_number: 备用通寄存器
---------------------------------------*/
void Write_SetChannelWord(float Data, AD9958_CFRMOOD_TypeDef Mode,
                          uint8_t CW_number) {
  uint32_t word = 0;
  uint8_t data[4] = {0x00, 0x00, 0x00, 0x00};

  switch (Mode) {
  /* amplitude_scale_factor = Amplitude * 2^10 */
  case 1:
    word = (uint32_t)(Data * 1023.0f) << 22;
    break;

  /* frequency_tuning_word = Frequency * 2^32 / (SystemClock * PLLDividerRatio)
   */
  case 2:
    word = (uint32_t)Data * 8.589934592;
    break;

  /* phase_offset_word = Phase * 2^14 / 360 */
  case 3:
    word = (uint32_t)(Data / 360.0f * 16384.0f) << 18;
    break;

  default:
    break;
  }

  data[0] = word >> 24;
  data[1] = word >> 16;
  data[2] = word >> 8;
  data[3] = word >> 0;

  WriteData_AD9959(FDW_ADD + CW_number, 4, data, 1);
}
/*---------------------------------------
函数功能：设置调制进制(2, 4, 8, 16)
Level: 		进制选择（0 ~ 3）
---------------------------------------*/
void Write_ModulationLevelSelection(uint8_t Level) {
  uint8_t data[3] = {0x00, 0x00, 0x00};

  Level &= 0x03;

  data[0] = ((uint8_t)20 << 2) | 0x80;
  data[1] = 0x50 | Level;

  WriteData_AD9959(FR1_ADD, 3, data, 1);
}
/*---------------------------------------
函数功能：设置调制模式
Mode: 		调制模式（AD9958_AMPLITUDE， AD9958_FREQUENCY， AD9958_PHASE）
NewState: 0,1
---------------------------------------*/
void Write_ModulationTypeConfiguration(AD9958_CFRMOOD_TypeDef Mode,
                                       AD9958_LinearSweep_TypeDef NewState) {
  uint8_t CFR_DATA[3] = {0x00, 0x00, 0x00}; // default Value = 0x000302

  NewState &= 0x01;

  CFR_DATA[0] = Mode << 6;
  CFR_DATA[1] = 0x03 | (NewState << 6);
  CFR_DATA[2] = 0x00;

  WriteData_AD9959(CFR_ADD, 3, CFR_DATA, 1);
}
/*---------------------------------------
函数功能：设置扫频模式
Channel： 通道选择
Rising：上升时间隔时间(纳秒)
Falling：下降时间隔时间(纳秒)
RisingFrequency：设置上升扫描每次增加的频率
FallingFrequency：设置下降扫描每次减少的频率
Freq：设置通道输出频率
Data：设置扫描终止频率
---------------------------------------*/
void LINEAR_SWEEP_MODE(AD9958_Channel_TypeDef Channel, // 通道选择
                       float Rising,          // 上升时间隔时间(纳秒)
                       float Falling,         // 下降时间隔时间(纳秒)
                       float RisingFrequency, // 设置上升扫描每次增加的频率
                       float FallingFrequency, // 设置下降扫描每次减少的频率
                       uint32_t Freq,          // 设置通道输出频率
                       float Data              // 设置扫描终止频率
) {
  float time = 0.0f;
  Write_ChoiceChannel(Channel);
  Write_ModulationLevelSelection(0x00);
  Write_ModulationTypeConfiguration(AD9958_FREQUENCY, LINEAR_OPEN);
  Write_SetBlankingTimeNs(Rising, Falling);
  Write_SetFrequencyRisingLSR(RisingFrequency);
  Write_SetFrequencyFallingLSR(FallingFrequency);
  Write_frequence(Freq);
  Write_SetChannelWord(Data, AD9958_FREQUENCY, 0x01);

  /* 当定时时间到期时,会触发中断,可以知道扫频过程完成。 */
  // time = Data * Rising / RisingFrequency * 0.001;
  // time += Data * Falling / FallingFrequency * 0.001;
  // timerSetTime(TIM2, time);
  // TIM2->DIER |= TIM_IT_Update;
}
