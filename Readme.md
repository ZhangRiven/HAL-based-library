<!--
 * @Author       : ZhangRiven
 * @Date         : 2023-08-18 15:10:13
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-08-18 15:24:44
 * @FilePath     : \Template\Readme.md
 * @Description  : 
-->
# 库、外设库

- 接口均使用STM32 HAL库
- 大部分库使用了面向对象
- 部分库使用C++

## Bsp
| 库名 | 包含文件名 | 测试 | 备注 |
| - | - | - | - |
| AD9851 | AD9851.c/.h | 可用 | 移植 |
| AD9954 | AD9854.c/.h | 可用 | 移植 |
| AD9958 | AD9858.c/.h | 未测试 | 移植 |
| jdy40 | jdy40.c/.h | 未测试 | 未完成，无线透传模块，串口通信 |
| KeyPad | KeyPad.hpp | 可用 | C++矩阵键盘 |
| lcd | lcd.hpp/lcd_font.h | 可用 | fsmc驱动lcd屏 |
| MFB | MFB.c/.h | 可用 | 移植，多功能按钮 |
| xd4051 | xd4051.c/.h | 可用 | 模拟开关 |
