/*
 * @Author       : ZhangRiven
 * @Date         : 2023-04-29 13:06:51
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-06-24 17:50:46
 * @Description  :
 */

#ifndef _MFB_H_
#define _MFB_H_
#include "task_app.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MULTI_CLICK 0   // 设置是否使能连击功能
#define LONG_CONTINUE 1 // 设置是否使能长按连续功能

typedef enum {
  eKeyLow,
  eKeyHigh,
} eKey;

typedef enum {
  eKeyNull,
  eKeyPress,
  eKeyRelease,
  eKeyLongPress,
#if LONG_CONTINUE
  eKeyLongContinnue,
#endif
  eKeyLongRelease,
} eKeyStatus;

typedef struct _Key_t KeyInfo_t;

typedef u8 (*pKeyCallBackFunction)(KeyInfo_t *keyInfo);

typedef struct _Key_CB_t {
  pKeyCallBackFunction ShortPressCB;
  pKeyCallBackFunction ShortReleaseCB;
  pKeyCallBackFunction LongPressCB;
#if LONG_CONTINUE
  pKeyCallBackFunction LongContinueCB;
#endif
  pKeyCallBackFunction LongReleaseCB;
} KeyCB_t;

typedef struct _Key_t {
  eKeyStatus status;
  uint8_t ucId;
  uint16_t usDebounce;
  uint16_t usShortPressDebounce;
  uint16_t usLongPressDebounce;
#if MULTI_CLICK
  uint8_t ucShortKeyTimes;
  uint16_t usReleaseDebounce;
  uint16_t usShortKeyReleaseDebounce;
#endif
#if LONG_CONTINUE
  uint16_t usLongContinueDebounce;
#endif
  KeyCB_t* pKeyCB;
} KeyInfo_t;

// 16bit Align

void KeyInit(void);
// void KeyScan(void);
void KeyProcess(void);


u8 KeyShortPress(KeyInfo_t *keyInfo);
u8 KeyShortRelease(KeyInfo_t *keyInfo);
u8 KeyLongPress(KeyInfo_t *keyInfo);
u8 KeyLongContinue(KeyInfo_t *keyInfo);
u8 KeyLongRelease(KeyInfo_t *keyInfo);

#ifdef __cplusplus
}
#endif

#endif
