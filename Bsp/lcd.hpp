/*
 * @Author       : ZhangRiven
 * @Date         : 2023-06-20 12:25:36
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-03 18:30:48
 * @FilePath     : \HAL_F407ZGT6\Bsp\lcd.hpp
 * @Description  : 代码源自正点原子，感谢正点原子
 */
#ifndef __LCD_HPP
#define __LCD_HPP
#include "task_app.hpp"

#include "lcd_font.h"

namespace LCD {

// /* 读时序的地址建立时间，单位：HCLK */
// constexpr static u32 FSMC_READ_AST = 0x0F;
// /* 读时序的数据建立时间，单位：HCLK */
// constexpr static u32 FSMC_READ_DST = 0x3C;
// /* 写时序的地址建立时间，单位：HCLK */
// constexpr static u32 FSMC_WRITE_AST = 0x03;
// /* 写时序的数据建立时间，单位：HCLK */
// constexpr static u32 FSMC_WRITE_DST = 0x03;

/* FSMC接口读写命令、数据 */
// #define FSMC_CMD_REG (*(volatile uint16_t *)FSMC_CMD_ADDR)
// #define FSMC_DAT_REG (*(volatile uint16_t *)FSMC_DAT_ADDR)
// static volatile uint16_t &FSMC_CMD_REG = *reinterpret_cast<volatile uint16_t
// *>(FSMC_CMD_ADDR); static volatile uint16_t &FSMC_DAT_REG =
// *reinterpret_cast<volatile uint16_t *>(FSMC_DAT_ADDR);
namespace base {
/* 所接FSMC的Bank */
// constexpr const static u32 FSMC_BANK = FSMC_NORSRAM_BANK4;
constexpr const static u32 FSMC_BANK_ADDR = 0x6C000000;
} // namespace base

struct CHIP_ID {
  static const u32 ILI9341_ID = 0x9341;
  static const u32 ST7789_ID = 0x8552;
};

enum class SCAN_DIRECTION {
  L2R_U2D = 0x0008, /* 从左到右，从上到下 */
  L2R_D2U = 0x0088, /* 从左到右，从下到上 */
  R2L_U2D = 0x0048, /* 从右到左，从上到下 */
  R2L_D2U = 0x00C8, /* 从右到左，从下到上 */
  U2D_L2R = 0x0028, /* 从上到下，从左到右 */
  U2D_R2L = 0x0068, /* 从上到下，从右到左 */
  D2U_L2R = 0x00A8, /* 从下到上，从左到右 */
  D2U_R2L = 0x00E8, /* 从下到上，从右到左 */
};

enum class DISPLAY_DIRECTION {
  _0 = 0x00, /* LCD顺时针旋转0°显示内容 */
  _90,       /* LCD顺时针旋转90°显示内容 */
  _180,      /* LCD顺时针旋转180°显示内容 */
  _270,      /* LCD顺时针旋转270°显示内容 */
};

enum class FONT_TYPE_SIZE {
#if (FONT_TYPE_SIZE_12 != 0)
  _12, /* 12号字体 */
#endif
#if (FONT_TYPE_SIZE_16 != 0)
  _16, /* 16号字体 */
#endif
#if (FONT_TYPE_SIZE_24 != 0)
  _24, /* 24号字体 */
#endif
#if (FONT_TYPE_SIZE_32 != 0)
  _32, /* 32号字体 */
#endif
};

enum class SHOW_NUM_MODE {
  NO_ZERO = 0x00, /* 数字高位0不显示 */
  ZERO,           /* 数字高位0显示 */
};

enum class COLOR_16BIT {
  WHITE = 0xFFFF,
  BLACK = 0x0000,
  BLUE = 0x001F,
  BRED = 0XF81F,
  GRED = 0XFFE0,
  GBLUE = 0X07FF,
  RED = 0xF800,
  MAGENTA = 0xF81F,
  GREEN = 0x07E0,
  CYAN = 0x7FFF,
  YELLOW = 0xFFE0,
  BROWN = 0XBC40,
  BRRED = 0XFC07,
  GRAY = 0X8430,
};

enum class LCD_STATUS {
  EOK = 0,    /* 没有错误 */
  ERROR = 1,  /* 错误 */
  EINVAL = 2, /* 非法参数 */
};
template <size_t Chip_Select_Pin> class lcd {
public:
  /* 初始化 */
  lcd &Init() {
    uint16_t chip_id;
    /* 获取驱动器ID */
    chip_id = get_chip_id();
    if ((chip_id != CHIP_ID::ILI9341_ID) && (chip_id != CHIP_ID::ST7789_ID)) {
      lcd_status_data.status = LCD_STATUS::ERROR;
    } else {
      lcd_status_data.chip_id = chip_id;
      lcd_status_data.width = LCD_SIZE::WIDTH;
      lcd_status_data.height = LCD_SIZE::HEIGHT;
    }
    reg_init();
    Set_disp_dir(DISPLAY_DIRECTION::_0);
    Clear(COLOR_16BIT::WHITE);
    Display_on();
    Backlight();
#if (USING_TOUCH != 0)
    touch_init();
#endif
    lcd_status_data.status = LCD_STATUS::EOK;
    return *this;
  }
  /* 获取LCD宽度 */
  uint16_t Get_lcd_width() { return lcd_status_data.width; }
  /* 获取LCD高度 */
  uint16_t Get_lcd_height() { return lcd_status_data.height; }
  /* LCD背光 */
  lcd &Backlight(u8 on = 1) {
    GPIO_PinState pin_status = static_cast<GPIO_PinState>(on);
    HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, pin_status);
    return *this;
  }
  /* 开启LCD显示 */
  lcd &Display_on() {
    fsmc_write_cmd(0x29);
    return *this;
  }
  /* 关闭LCD显示 */
  lcd &Display_off() {
    fsmc_write_cmd(0x28);
    return *this;
  }
  /* 设置LCD扫描方向 */
  lcd &Set_scan_dir(SCAN_DIRECTION scan_dir) {
    SCAN_DIRECTION reg36;

    switch (lcd_status_data.disp_dir) {
    case DISPLAY_DIRECTION::_0: {
      reg36 = scan_dir;
      break;
    }
    case DISPLAY_DIRECTION::_90: {
      switch (scan_dir) {
      case SCAN_DIRECTION::L2R_U2D:
        reg36 = SCAN_DIRECTION::U2D_R2L;
        break;

      case SCAN_DIRECTION::L2R_D2U:
        reg36 = SCAN_DIRECTION::U2D_L2R;
        break;

      case SCAN_DIRECTION::R2L_U2D:
        reg36 = SCAN_DIRECTION::D2U_R2L;
        break;

      case SCAN_DIRECTION::R2L_D2U:
        reg36 = SCAN_DIRECTION::D2U_L2R;
        break;

      case SCAN_DIRECTION::U2D_L2R:
        reg36 = SCAN_DIRECTION::R2L_U2D;
        break;

      case SCAN_DIRECTION::U2D_R2L:
        reg36 = SCAN_DIRECTION::R2L_D2U;
        break;

      case SCAN_DIRECTION::D2U_L2R:
        reg36 = SCAN_DIRECTION::L2R_U2D;
        break;

      case SCAN_DIRECTION::D2U_R2L:
        reg36 = SCAN_DIRECTION::L2R_D2U;
        break;

      default:
        lcd_status_data.status = LCD_STATUS::EINVAL;
      }
      break;
    }
    case DISPLAY_DIRECTION::_180: {
      switch (scan_dir) {
      case SCAN_DIRECTION::L2R_U2D:
        reg36 = SCAN_DIRECTION::R2L_D2U;
        break;

      case SCAN_DIRECTION::L2R_D2U:
        reg36 = SCAN_DIRECTION::R2L_U2D;
        break;

      case SCAN_DIRECTION::R2L_U2D:
        reg36 = SCAN_DIRECTION::L2R_D2U;
        break;

      case SCAN_DIRECTION::R2L_D2U:
        reg36 = SCAN_DIRECTION::L2R_U2D;
        break;

      case SCAN_DIRECTION::U2D_L2R:
        reg36 = SCAN_DIRECTION::D2U_R2L;
        break;

      case SCAN_DIRECTION::U2D_R2L:
        reg36 = SCAN_DIRECTION::D2U_L2R;
        break;

      case SCAN_DIRECTION::D2U_L2R:
        reg36 = SCAN_DIRECTION::U2D_R2L;
        break;

      case SCAN_DIRECTION::D2U_R2L:
        reg36 = SCAN_DIRECTION::U2D_L2R;
        break;

      default:
        lcd_status_data.status = LCD_STATUS::EINVAL;
      }
      break;
    }
    case DISPLAY_DIRECTION::_270: {
      switch (scan_dir) {
      case SCAN_DIRECTION::L2R_U2D: {
        reg36 = SCAN_DIRECTION::D2U_L2R;
        break;
      }
      case SCAN_DIRECTION::L2R_D2U: {
        reg36 = SCAN_DIRECTION::D2U_R2L;
        break;
      }
      case SCAN_DIRECTION::R2L_U2D: {
        reg36 = SCAN_DIRECTION::U2D_L2R;
        break;
      }
      case SCAN_DIRECTION::R2L_D2U: {
        reg36 = SCAN_DIRECTION::U2D_R2L;
        break;
      }
      case SCAN_DIRECTION::U2D_L2R: {
        reg36 = SCAN_DIRECTION::L2R_D2U;
        break;
      }
      case SCAN_DIRECTION::U2D_R2L: {
        reg36 = SCAN_DIRECTION::L2R_U2D;
        break;
      }
      case SCAN_DIRECTION::D2U_L2R: {
        reg36 = SCAN_DIRECTION::R2L_D2U;
        break;
      }
      case SCAN_DIRECTION::D2U_R2L: {
        reg36 = SCAN_DIRECTION::R2L_U2D;
        break;
      }
      default: {
        lcd_status_data.status = LCD_STATUS::EINVAL;
      }
      }
      break;
    }
    default: {
      lcd_status_data.status = LCD_STATUS::ERROR;
    }
    }

    lcd_status_data.scan_dir = reg36;
    fsmc_write_reg(0x36, static_cast<u16>(reg36));
    set_column_address(0, lcd_status_data.width - 1);
    set_page_address(0, lcd_status_data.height - 1);

    lcd_status_data.status = LCD_STATUS::EOK;
    return *this;
  }
  /* 设置LCD显示方向 */
  lcd &Set_disp_dir(DISPLAY_DIRECTION disp_dir) {
    switch (disp_dir) {
    case DISPLAY_DIRECTION::_0:
      lcd_status_data.width = LCD_SIZE::WIDTH;
      lcd_status_data.height = LCD_SIZE::HEIGHT;
      break;

    case DISPLAY_DIRECTION::_90:
      lcd_status_data.width = LCD_SIZE::HEIGHT;
      lcd_status_data.height = LCD_SIZE::WIDTH;
      break;

    case DISPLAY_DIRECTION::_180:
      lcd_status_data.width = LCD_SIZE::WIDTH;
      lcd_status_data.height = LCD_SIZE::HEIGHT;
      break;

    case DISPLAY_DIRECTION::_270:
      lcd_status_data.width = LCD_SIZE::HEIGHT;
      lcd_status_data.height = LCD_SIZE::WIDTH;
      break;

    default:
      lcd_status_data.status = LCD_STATUS::EINVAL;
    }

    lcd_status_data.disp_dir = disp_dir;
    Set_scan_dir(SCAN_DIRECTION::L2R_U2D);

    lcd_status_data.status = LCD_STATUS::EOK;
    return *this;
  }
  /* 获取LCD扫描方向 */
  SCAN_DIRECTION Get_scan_dir() { return lcd_status_data.scan_dir; }
  /* 获取LCD显示方向 */
  DISPLAY_DIRECTION Get_disp_dir() { return lcd_status_data.disp_dir; }
  /* LCD区域填充 */
  lcd &Fill(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height,
            COLOR_16BIT color) {
    uint16_t x_index;
    uint16_t y_index;
    uint16_t xe = xs + width;
    uint16_t ye = ys + height;
    set_column_address(xs, xe);
    set_page_address(ys, ye);
    start_write_memory();
    for (y_index = ys; y_index <= ye; y_index++) {
      for (x_index = xs; x_index <= xe; x_index++) {
        fsmc_write_dat(static_cast<u16>(color));
      }
    }
    return *this;
  }

  /* LCD清屏 */
  lcd &Clear(COLOR_16BIT color) {
    Fill(0, 0, lcd_status_data.width - 1, lcd_status_data.height - 1, color);
    return *this;
  }
  /* LCD画点 */
  lcd &Draw_point(uint16_t x, uint16_t y, COLOR_16BIT color) {
    set_column_address(x, x);
    set_page_address(y, y);
    start_write_memory();
    fsmc_write_dat(static_cast<u16>(color));
    return *this;
  }
  /* LCD读点 */
  uint16_t Read_point(uint16_t x, uint16_t y) {
    COLOR_16BIT color;
    uint8_t color_r;
    uint8_t color_g;
    uint8_t color_b;

    if ((x >= lcd_status_data.width) || (y >= lcd_status_data.height)) {
      lcd_status_data.status = LCD_STATUS::EINVAL;
    }

    set_column_address(x, x);
    set_page_address(y, y);
    start_read_memory();

    u16 u16_color;
    u16_color = fsmc_read_dat(); /* Dummy */
    u16_color = fsmc_read_dat(); /* [15:11]: R, [7:2]:G */
    color_r = (uint8_t)(u16_color >> 11) & 0x1F;
    color_g = (uint8_t)(u16_color >> 2) & 0x3F;
    u16_color = fsmc_read_dat(); /* [15:11]: B */
    color_b = (uint8_t)(u16_color >> 11) & 0x1F;

    color = static_cast<COLOR_16BIT>(u16_color);
    return (uint16_t)(color_r << 11) | (color_g << 5) | color_b;
  }

  /* LCD画线段 */
  lcd &Draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                 COLOR_16BIT color) {
    uint16_t x_delta;
    uint16_t y_delta;
    int16_t x_sign;
    int16_t y_sign;
    int16_t error;
    int16_t error2;

    x_delta = (x1 < x2) ? (x2 - x1) : (x1 - x2);
    y_delta = (y1 < y2) ? (y2 - y1) : (y1 - y2);
    x_sign = (x1 < x2) ? 1 : -1;
    y_sign = (y1 < y2) ? 1 : -1;
    error = x_delta - y_delta;

    Draw_point(x2, y2, color);

    while ((x1 != x2) || (y1 != y2)) {
      Draw_point(x1, y1, color);

      error2 = error << 1;
      if (error2 > -y_delta) {
        error -= y_delta;
        x1 += x_sign;
      }

      if (error2 < x_delta) {
        error += x_delta;
        y1 += y_sign;
      }
    }
    return *this;
  }
  /* LCD画矩形框 */
  lcd &Draw_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2,
                 COLOR_16BIT color) {
    Draw_line(x1, y1, x2, y1, color);
    Draw_line(x1, y2, x2, y2, color);
    Draw_line(x1, y1, x1, y2, color);
    Draw_line(x2, y1, x2, y2, color);
    return *this;
  }
  /* LCD画圆形框 */
  lcd &Draw_circle(uint16_t x, uint16_t y, uint16_t r, COLOR_16BIT color) {
    int32_t x_t;
    int32_t y_t;
    int32_t error;
    int32_t error2;

    x_t = -r;
    y_t = 0;
    error = 2 - 2 * r;

    do {
      Draw_point(x - x_t, y + y_t, color);
      Draw_point(x + x_t, y + y_t, color);
      Draw_point(x + x_t, y - y_t, color);
      Draw_point(x - x_t, y - y_t, color);

      error2 = error;
      if (error2 <= y_t) {
        y_t++;
        error = error + (y_t * 2 + 1);
        if ((-x_t == y_t) && (error2 <= x_t)) {
          error2 = 0;
        }
      }

      if (error2 > x_t) {
        x_t++;
        error = error + (x_t * 2 + 1);
      }
    } while (x_t <= 0);
    return *this;
  }
  /* LCD显示1个字符 */
  lcd &Show_char(uint16_t x, uint16_t y, char ch, FONT_TYPE_SIZE font,
                 COLOR_16BIT color) {
    const uint8_t *ch_code;
    uint8_t ch_width;
    uint8_t ch_height;
    uint8_t ch_size;
    uint8_t ch_offset;
    uint8_t byte_index;
    uint8_t byte_code;
    uint8_t bit_index;
    uint8_t width_index = 0;
    uint8_t height_index = 0;

    ch_offset = ch - ' ';

    switch (font) {
#if (FONT_TYPE_SIZE_12 != 0)
    case FONT_TYPE_SIZE::_12: {
      ch_code = font_1206[ch_offset];
      ch_width = FONT_12_CHAR_WIDTH;
      ch_height = FONT_12_CHAR_HEIGHT;
      ch_size = FONT_12_CHAR_SIZE;
      break;
    }
#endif
#if (FONT_TYPE_SIZE_16 != 0)
    case FONT_TYPE_SIZE::_16: {
      ch_code = font_1608[ch_offset];
      ch_width = FONT_16_CHAR_WIDTH;
      ch_height = FONT_16_CHAR_HEIGHT;
      ch_size = FONT_16_CHAR_SIZE;
      break;
    }
#endif
#if (FONT_TYPE_SIZE_24 != 0)
    case FONT_TYPE_SIZE::_24: {
      ch_code = font_2412[ch_offset];
      ch_width = FONT_24_CHAR_WIDTH;
      ch_height = FONT_24_CHAR_HEIGHT;
      ch_size = FONT_24_CHAR_SIZE;
      break;
    }
#endif
#if (FONT_TYPE_SIZE_32 != 0)
    case FONT_TYPE_SIZE::_32: {
      ch_code = font_3216[ch_offset];
      ch_width = FONT_32_CHAR_WIDTH;
      ch_height = FONT_32_CHAR_HEIGHT;
      ch_size = FONT_32_CHAR_SIZE;
      break;
    }
#endif
    default: {
      return *this;
    }
    }

    if ((x + ch_width > lcd_status_data.width) ||
        (y + ch_height > lcd_status_data.height)) {
      return *this;
    }

    for (byte_index = 0; byte_index < ch_size; byte_index++) {
      byte_code = ch_code[byte_index];
      for (bit_index = 0; bit_index < 8; bit_index++) {
        if ((byte_code & 0x80) != 0) {
          Draw_point(x + width_index, y + height_index, color);
        }
        width_index++;
        if (width_index == ch_width) {
          width_index = 0;
          height_index++;
          break;
        }
        byte_code <<= 1;
      }
    }
    return *this;
  }
  /* LCD显示字符串 */
  lcd &Show_string(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                   const char *str, FONT_TYPE_SIZE font, COLOR_16BIT color,
                   uint8_t clear = 0,
                   COLOR_16BIT back_color = COLOR_16BIT::WHITE) {
    uint8_t ch_width;
    uint8_t ch_height;
    uint16_t x_raw;
    uint16_t y_raw;
    uint16_t x_limit;
    uint16_t y_limit;
    if (clear) {
      Fill(x, y, width, height, back_color);
    }
    switch (font) {
#if (FONT_TYPE_SIZE_12 != 0)
    case FONT_TYPE_SIZE::_12: {
      ch_width = FONT_12_CHAR_WIDTH;
      ch_height = FONT_12_CHAR_HEIGHT;
      break;
    }
#endif
#if (FONT_TYPE_SIZE_16 != 0)
    case FONT_TYPE_SIZE::_16: {
      ch_width = FONT_16_CHAR_WIDTH;
      ch_height = FONT_16_CHAR_HEIGHT;
      break;
    }
#endif
#if (FONT_TYPE_SIZE_24 != 0)
    case FONT_TYPE_SIZE::_24: {
      ch_width = FONT_24_CHAR_WIDTH;
      ch_height = FONT_24_CHAR_HEIGHT;
      break;
    }
#endif
#if (FONT_TYPE_SIZE_32 != 0)
    case FONT_TYPE_SIZE::_32: {
      ch_width = FONT_32_CHAR_WIDTH;
      ch_height = FONT_32_CHAR_HEIGHT;
      break;
    }
#endif
    default: {
      return *this;
    }
    }

    x_raw = x;
    y_raw = y;
    x_limit = ((x + width + 1) > lcd_status_data.width) ? lcd_status_data.width
                                                        : (x + width + 1);
    y_limit = ((y + height + 1) > lcd_status_data.height)
                  ? lcd_status_data.height
                  : (y + height + 1);

    while ((*str >= ' ') && (*str <= '~')) {
      if (x + ch_width >= x_limit) {
        x = x_raw;
        y += ch_height;
      }

      if (y + ch_height >= y_limit) {
        y = x_raw;
        x = y_raw;
      }

      Show_char(x, y, *str, font, color);

      x += ch_width;
      str++;
    }
    return *this;
  }
  /* LCD显示数字，可控制显示高位0 */
  lcd &Show_xnum(uint16_t x, uint16_t y, uint32_t num, uint8_t len,
                 SHOW_NUM_MODE mode, FONT_TYPE_SIZE font, COLOR_16BIT color) {
    uint8_t ch_width;
    uint8_t len_index;
    uint8_t num_index;
    uint8_t first_nozero = 0;
    char pad;

    switch (font) {
#if (FONT_TYPE_SIZE_12 != 0)
    case FONT_TYPE_SIZE::_12: {
      ch_width = FONT_12_CHAR_WIDTH;
      break;
    }
#endif
#if (FONT_TYPE_SIZE_16 != 0)
    case FONT_TYPE_SIZE::_16: {
      ch_width = FONT_16_CHAR_WIDTH;
      break;
    }
#endif
#if (FONT_TYPE_SIZE_24 != 0)
    case FONT_TYPE_SIZE::_24: {
      ch_width = FONT_24_CHAR_WIDTH;
      break;
    }
#endif
#if (FONT_TYPE_SIZE_32 != 0)
    case FONT_TYPE_SIZE::_32: {
      ch_width = FONT_32_CHAR_WIDTH;
      break;
    }
#endif
    default: {
      return *this;
    }
    }

    switch (mode) {
    case SHOW_NUM_MODE::NO_ZERO: {
      pad = ' ';
      break;
    }
    case SHOW_NUM_MODE::ZERO: {
      pad = '0';
      break;
    }
    default: {
      return *this;
    }
    }

    for (len_index = 0; len_index < len; len_index++) {
      num_index = (num / pow(10, len - len_index - 1)) % 10;
      if ((first_nozero == 0) && (len_index < (len - 1))) {
        if (num_index == 0) {
          Show_char(x + ch_width * len_index, y, pad, font, color);
          continue;
        } else {
          first_nozero = 1;
        }
      }

      Show_char(x + ch_width * len_index, y, num_index + '0', font, color);
    }
    return *this;
  }
  /* LCD显示数字，不显示高位0 */
  lcd &Show_num(uint16_t x, uint16_t y, uint32_t num, uint8_t len,
                FONT_TYPE_SIZE font, COLOR_16BIT color) {
    Show_xnum(x, y, num, len, SHOW_NUM_MODE::NO_ZERO, font, color);
    return *this;
  }
  /* LCD图片 */
  lcd &Show_pic(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                uint8_t *pic) {
    uint16_t x_index;
    uint16_t y_index;

    if ((x + width > LCD_SIZE::WIDTH) || (y + height > LCD_SIZE::HEIGHT)) {
      return *this;
    }

    set_column_address(x, x + width - 1);
    set_page_address(y, y + height - 1);
    start_write_memory();
    for (y_index = y; y_index <= (y + height); y_index++) {
      for (x_index = x; x_index <= (x + width); x_index++) {
        fsmc_write_dat(*pic);
        pic++;
      }
    }
    return *this;
  }

private:
  /* 通过FSMC接口写命令 */
  inline void fsmc_write_cmd(volatile uint16_t cmd) { FSMC_CMD_REG = cmd; }
  /* 通过FSMC接口写数据 */
  inline void fsmc_write_dat(volatile uint16_t dat) { FSMC_DAT_REG = dat; }
  /* 通过FSMC接口写寄存器 */
  inline void fsmc_write_reg(volatile uint16_t reg, volatile uint16_t dat) {
    FSMC_CMD_REG = reg;
    FSMC_DAT_REG = dat;
  }
  /* 通过FSMC接口读数据 */
  inline uint16_t fsmc_read_dat(void) {
    uint16_t dat;
    __nop();
    __nop();
    dat = FSMC_DAT_REG;
    return dat;
  }

  /* lcd状态数据结构体 */
  struct {
    uint16_t chip_id;           /* 驱动器ID */
    uint16_t width;             /* LCD宽度 */
    uint16_t height;            /* LCD高度 */
    SCAN_DIRECTION scan_dir;    /* LCD扫描方向 */
    DISPLAY_DIRECTION disp_dir; /* LCD显示方向 */
    LCD_STATUS status;          /* lcd状态 */
  } lcd_status_data = {0};
  /* 读芯片ID */
  uint16_t get_chip_id() {
    uint16_t chip_id;

    /* 尝试获取ILI9341_ID */
    fsmc_write_cmd(0xD3);
    chip_id = fsmc_read_dat();
    chip_id = fsmc_read_dat();
    chip_id = fsmc_read_dat() << 8;
    chip_id |= (fsmc_read_dat()) & 0x00FF;

    if (chip_id != CHIP_ID::ILI9341_ID) // ST7789_ID
    {
      /* 尝试获取ST7789_ID */
      fsmc_write_cmd(0x04);
      chip_id = fsmc_read_dat();
      chip_id = fsmc_read_dat();
      chip_id = fsmc_read_dat() << 8;
      chip_id |= (fsmc_read_dat()) & 0x00FF;
    }

    return chip_id;
  }
  /* 寄存器初始化 */
  void reg_init() {
    if (lcd_status_data.chip_id == CHIP_ID::ILI9341_ID) {
      fsmc_write_cmd(0xCF);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0xC1);
      fsmc_write_dat(0x30);
      fsmc_write_cmd(0xED);
      fsmc_write_dat(0x64);
      fsmc_write_dat(0x03);
      fsmc_write_dat(0x12);
      fsmc_write_dat(0x81);
      fsmc_write_cmd(0xE8);
      fsmc_write_dat(0x85);
      fsmc_write_dat(0x10);
      fsmc_write_dat(0x7A);
      fsmc_write_cmd(0xCB);
      fsmc_write_dat(0x39);
      fsmc_write_dat(0x2C);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x34);
      fsmc_write_dat(0x02);
      fsmc_write_cmd(0xF7);
      fsmc_write_dat(0x20);
      fsmc_write_cmd(0xEA);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x00);
      fsmc_write_cmd(0xC0);
      fsmc_write_dat(0x1B);
      fsmc_write_cmd(0xC1);
      fsmc_write_dat(0x01);
      fsmc_write_cmd(0xC5);
      fsmc_write_dat(0x30);
      fsmc_write_dat(0x30);
      fsmc_write_cmd(0xC7);
      fsmc_write_dat(0xB7);
      fsmc_write_cmd(0x36);
      fsmc_write_dat(0x48);
      fsmc_write_cmd(0x3A);
      fsmc_write_dat(0x55);
      fsmc_write_cmd(0xB1);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x1A);
      fsmc_write_cmd(0xB6);
      fsmc_write_dat(0x0A);
      fsmc_write_dat(0xA2);
      fsmc_write_cmd(0xF2);
      fsmc_write_dat(0x00);
      fsmc_write_cmd(0x26);
      fsmc_write_dat(0x01);
      fsmc_write_cmd(0xE0);
      fsmc_write_dat(0x0F);
      fsmc_write_dat(0x2A);
      fsmc_write_dat(0x28);
      fsmc_write_dat(0x08);
      fsmc_write_dat(0x0E);
      fsmc_write_dat(0x08);
      fsmc_write_dat(0x54);
      fsmc_write_dat(0xA9);
      fsmc_write_dat(0x43);
      fsmc_write_dat(0x0A);
      fsmc_write_dat(0x0F);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x00);
      fsmc_write_cmd(0xE1);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x15);
      fsmc_write_dat(0x17);
      fsmc_write_dat(0x07);
      fsmc_write_dat(0x11);
      fsmc_write_dat(0x06);
      fsmc_write_dat(0x2B);
      fsmc_write_dat(0x56);
      fsmc_write_dat(0x3C);
      fsmc_write_dat(0x05);
      fsmc_write_dat(0x10);
      fsmc_write_dat(0x0F);
      fsmc_write_dat(0x3F);
      fsmc_write_dat(0x3F);
      fsmc_write_dat(0x0F);
      fsmc_write_cmd(0x2B);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x01);
      fsmc_write_dat(0x3F);
      fsmc_write_cmd(0x2A);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0xEF);
      fsmc_write_cmd(0x11);
      delay_ms(100);
      fsmc_write_cmd(0x29);
    } else if (lcd_status_data.chip_id == CHIP_ID::ST7789_ID) {
      fsmc_write_cmd(0x11);
      delay_ms(120);
      fsmc_write_cmd(0x36);
      fsmc_write_dat(0x00);
      fsmc_write_cmd(0x3A);
      fsmc_write_dat(0x05);
      fsmc_write_cmd(0xB2);
      fsmc_write_dat(0x0C);
      fsmc_write_dat(0x0C);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x33);
      fsmc_write_dat(0x33);
      fsmc_write_cmd(0xB7);
      fsmc_write_dat(0x35);
      fsmc_write_cmd(0xBB);
      fsmc_write_dat(0x32);
      fsmc_write_cmd(0xC0);
      fsmc_write_dat(0x0C);
      fsmc_write_cmd(0xC2);
      fsmc_write_dat(0x01);
      fsmc_write_cmd(0xC3);
      fsmc_write_dat(0x10);
      fsmc_write_cmd(0xC4);
      fsmc_write_dat(0x20);
      fsmc_write_cmd(0xC6);
      fsmc_write_dat(0x0F);
      fsmc_write_cmd(0xD0);
      fsmc_write_dat(0xA4);
      fsmc_write_dat(0xA1);
      fsmc_write_cmd(0xE0);
      fsmc_write_dat(0xD0);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x02);
      fsmc_write_dat(0x07);
      fsmc_write_dat(0x0A);
      fsmc_write_dat(0x28);
      fsmc_write_dat(0x32);
      fsmc_write_dat(0x44);
      fsmc_write_dat(0x42);
      fsmc_write_dat(0x06);
      fsmc_write_dat(0x0E);
      fsmc_write_dat(0x12);
      fsmc_write_dat(0x14);
      fsmc_write_dat(0x17);
      fsmc_write_cmd(0xE1);
      fsmc_write_dat(0xD0);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x02);
      fsmc_write_dat(0x07);
      fsmc_write_dat(0x0A);
      fsmc_write_dat(0x28);
      fsmc_write_dat(0x31);
      fsmc_write_dat(0x54);
      fsmc_write_dat(0x47);
      fsmc_write_dat(0x0E);
      fsmc_write_dat(0x1C);
      fsmc_write_dat(0x17);
      fsmc_write_dat(0x1B);
      fsmc_write_dat(0x1E);
      fsmc_write_cmd(0x2A);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0xEF);
      fsmc_write_cmd(0x2B);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x00);
      fsmc_write_dat(0x01);
      fsmc_write_dat(0x3F);
      fsmc_write_cmd(0x29);
    }
  }
  /* 设置列地址 */
  void set_column_address(uint16_t sc, uint16_t ec) {
    fsmc_write_cmd(0x2A);
    fsmc_write_dat((uint8_t)(sc >> 8) & 0xFF);
    fsmc_write_dat((uint8_t)sc & 0xFF);
    fsmc_write_dat((uint8_t)(ec >> 8) & 0xFF);
    fsmc_write_dat((uint8_t)ec & 0xFF);
  }
  /* 设置页地址 */
  void set_page_address(uint16_t sp, uint16_t ep) {
    fsmc_write_cmd(0x2B);
    fsmc_write_dat((uint8_t)(sp >> 8) & 0xFF);
    fsmc_write_dat((uint8_t)sp & 0xFF);
    fsmc_write_dat((uint8_t)(ep >> 8) & 0xFF);
    fsmc_write_dat((uint8_t)ep & 0xFF);
  }
  /* 开始写显存 */
  void start_write_memory() { fsmc_write_cmd(0x2C); }
  /* 开始读显存 */
  void start_read_memory() { fsmc_write_cmd(0x2E); }
  /* 幂函数，x^y */
  uint32_t pow(uint8_t x, uint8_t y) {
    uint8_t loop;
    uint32_t res = 1;
    for (loop = 0; loop < y; loop++) {
      res *= x;
    }
    return res;
  }

  struct LCD_SIZE {
    static const u16 WIDTH = 240;
    static const u16 HEIGHT = 320;
  };

  constexpr const static u32 FSMC_CMD_ADDR =
      (base::FSMC_BANK_ADDR | (((1U << Chip_Select_Pin) - 1) << 1));
  constexpr const static u32 FSMC_DAT_ADDR =
      (base::FSMC_BANK_ADDR | ((1U << Chip_Select_Pin) << 1));

  /* FSMC接口读写命令、数据地址 */
  volatile uint16_t &FSMC_CMD_REG =
      *reinterpret_cast<volatile uint16_t *>(FSMC_CMD_ADDR);
  volatile uint16_t &FSMC_DAT_REG =
      *reinterpret_cast<volatile uint16_t *>(FSMC_DAT_ADDR);
};

} // namespace LCD

#endif