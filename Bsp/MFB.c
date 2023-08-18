/*
 * @Author       : ZhangRiven
 * @Date         : 2023-06-23 11:24:20
 * @LastEditors  : ZhangRiven
 * @LastEditTime : 2023-07-23 21:56:41
 * @FilePath     : \AD9954_HAL_F407ZGT6\Bsp\MFB.c
 * @Description  :
 */
#include "MFB.h"

#define KEY_CNT 3                                                 // 按键数
#define KEY_BUF_CNT ((KEY_CNT >> 4) + ((KEY_CNT & 0x0F) ? 1 : 0)) // 缓存数

u8 KeyScanBuf[KEY_BUF_CNT];
KeyInfo_t KeyInfo[KEY_CNT];

__weak u8 KeyShortPress(KeyInfo_t *keyInfo) { return 0; }
__weak u8 KeyShortRelease(KeyInfo_t *keyInfo) { return 0; }
__weak u8 KeyLongPress(KeyInfo_t *keyInfo) { return 0; }
__weak u8 KeyLongContinue(KeyInfo_t *keyInfo) { return 0; }
__weak u8 KeyLongRelease(KeyInfo_t *keyInfo) { return 0; }

KeyCB_t KeyCbBuf = {
    KeyShortPress,   KeyShortRelease, KeyLongPress,
#if LONG_CONTINUE
    KeyLongContinue,
#endif
    KeyLongRelease,
};

void KeyInit(void) {
  for (int i = 0; i < KEY_CNT; i++) {
    KeyInfo[i].status = eKeyNull;
    KeyInfo[i].ucId = i;
    KeyInfo[i].usDebounce = 0;           // 按下计数
    KeyInfo[i].usShortPressDebounce = 2; // 短按去抖 *10ms
    KeyInfo[i].usLongPressDebounce = 50; // 长按去抖
#if MULTI_CLICK
    KeyInfo[i].ucShortKeyTimes = 0;
    KeyInfo[i].usReleaseDebounce = 0;
    KeyInfo[i].usShortKeyReleaseDebounce = 30;
#endif
#if LONG_CONTINUE
    KeyInfo[i].usLongContinueDebounce = 0;
#endif
    KeyInfo[i].pKeyCB = &KeyCbBuf;
  }
}

void KeyScan(void) {
  KeyScanBuf[0] = HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) |
                  HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) << 1 |
                  HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) << 2;
}

void KeyDetect(u8 *pKey, KeyInfo_t *KeyInfo, u8 KeyCnt) {
  for (int i = 0; i < KeyCnt; i++) {
    if ((pKey[i >> 3] & (0x01 << (i & 0x07))) == eKeyLow) { // 按下


      
      KeyInfo[i].usDebounce++;                  // 按下计时更新
      if (KeyInfo[i].status == eKeyLongPress) { // 长按状态
        
#if LONG_CONTINUE
        KeyInfo[i].status = eKeyLongContinnue;
        KeyInfo[i].usLongContinueDebounce =
            KeyInfo[i].usDebounce + KeyInfo[i].usLongPressDebounce;

      } else if ((KeyInfo[i].status == eKeyLongContinnue) &&
                 (KeyInfo[i].usLongContinueDebounce == KeyInfo[i].usDebounce)) {
        
        if (KeyInfo[i].pKeyCB->LongContinueCB != NULL) {
          KeyInfo[i].pKeyCB->LongContinueCB(&KeyInfo[i]);
        }
        KeyInfo[i].usLongContinueDebounce =
            KeyInfo[i].usDebounce + KeyInfo[i].usLongPressDebounce;
#endif

      } else if ((KeyInfo[i].status != eKeyLongPress) &&
                 (KeyInfo[i].usDebounce == KeyInfo[i].usLongPressDebounce)) {
        KeyInfo[i].status = eKeyLongPress;
        if (KeyInfo[i].pKeyCB->LongPressCB != NULL) {
          KeyInfo[i].pKeyCB->LongPressCB(&KeyInfo[i]);
        }
      } else if (KeyInfo[i].usDebounce == KeyInfo[i].usShortPressDebounce) {
        KeyInfo[i].status = eKeyPress;
        if (KeyInfo[i].pKeyCB->ShortPressCB != NULL) {
          KeyInfo[i].pKeyCB->ShortPressCB(&KeyInfo[i]);
        }
#if MULTI_CLICK
        KeyInfo[i].ucShortKeyTimes++;
        KeyInfo[i].usReleaseDebounce = 0;
#endif
      }



    } else {
      if (KeyInfo[i].status == eKeyPress) {
#if MULTI_CLICK
        KeyInfo[i].usReleaseDebounce++;
        if (KeyInfo[i].usReleaseDebounce >
            KeyInfo[i].usShortKeyReleaseDebounce) {
          // short key release
          if (KeyInfo[i].pKeyCB->ShortReleaseCB != NULL) {
            KeyInfo[i].pKeyCB->ShortReleaseCB(&KeyInfo[i]);
            KeyInfo[i].ucShortKeyTimes = 0;
            KeyInfo[i].usReleaseDebounce = 0;
            KeyInfo[i].status = eKeyNull;
          }
        }
#else
        if (KeyInfo[i].pKeyCB->ShortReleaseCB != NULL) {
          KeyInfo[i].pKeyCB->ShortReleaseCB(&KeyInfo[i]);
          KeyInfo[i].status = eKeyNull;
        }
#endif
      }
#if LONG_CONTINUE
      else if ((KeyInfo[i].status == eKeyLongContinnue) ||
               (KeyInfo[i].status == eKeyLongPress)) {
        if (KeyInfo[i].pKeyCB->LongReleaseCB != NULL) {
          KeyInfo[i].pKeyCB->LongReleaseCB(&KeyInfo[i]);
#if MULTI_CLICK
          KeyInfo[i].ucShortKeyTimes = 0;
          KeyInfo[i].usReleaseDebounce = 0;
#endif
          KeyInfo[i].status = eKeyNull;
        }
      }
#else
      else if (KeyInfo[i].status == eKeyLongPress) {
        if (KeyInfo[i].pKeyCB->LongReleaseCB != NULL) {
          KeyInfo[i].pKeyCB->LongReleaseCB(&KeyInfo[i]);
#if MULTI_CLICK
          KeyInfo[i].ucShortKeyTimes = 0;
          KeyInfo[i].usReleaseDebounce = 0;
#endif
          KeyInfo[i].status = eKeyNull;
        }
      }
#endif
      KeyInfo[i].usDebounce = 0;
    }
  }
}

void keyTask(u8 *keyState, KeyInfo_t *keyInfoArray, u8 keyCnt) {
  for (u8 i = 0; i < keyCnt; i++) {
    if ((keyState[i >> 3] & (0x01 << (i & 0x07))) == eKeyLow) { // 按下
      keyInfoArray[i].usDebounce++; // 更新本次按下时长


      



    } else {//未按下
      
    }
  }
}

void KeyProcess(void) {
  KeyScan();
  KeyDetect(KeyScanBuf, KeyInfo, KEY_CNT);
}
