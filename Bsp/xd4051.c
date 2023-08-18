#include "xd4051.h"

xd4051_def xd4051 = {
    {xd4051_A_GPIO_Port, xd4051_B_GPIO_Port, xd4051_C_GPIO_Port,
     xd4051_INH_GPIO_Port},
    {xd4051_A_Pin, xd4051_B_Pin, xd4051_C_Pin, xd4051_INH_Pin}};

void gpio_set_level(GPIO_TypeDef *Port, uint16_t Pin, uint8_t level) {
  HAL_GPIO_WritePin(Port, Pin, level);
}

void xd4051_swtich_channel(xd4051_def *chip, uint8_t channel) {
  if (channel < 8) {
    gpio_set_level(chip->Port[3], chip->Pin[3], 0);
  } else {
    gpio_set_level(chip->Port[3], chip->Pin[3], 1);
    return;
  }
  for (uint8_t i = 0; i < 3; i++) {
    u8 gpio_level = (channel >> i) & 0x01;
    gpio_set_level(chip->Port[i], chip->Pin[i], gpio_level);
  }
}