#include "task_key.h"

#include "tim.h"
KeyPad<GPIO_TypeDef *, uint16_t> keypad;

//void Key_scan_Timer_Callback(void *argument);
//osTimerId_t Key_scan_TimerHandle;
//const osTimerAttr_t Key_scan_Timer_attributes = {.name = "Key_scan_Timer"};

void timer_Key_Scan_Init() {
  keypad.add_row_pin(GPIOE, GPIO_PIN_0)
      .add_row_pin(GPIOB, GPIO_PIN_8)
      .add_row_pin(GPIOB, GPIO_PIN_6)
      .add_row_pin(GPIOB, GPIO_PIN_4)
      .add_column_pin(GPIOG, GPIO_PIN_15)
      .add_column_pin(GPIOG, GPIO_PIN_13)
      .add_column_pin(GPIOG, GPIO_PIN_11)
      .add_column_pin(GPIOG, GPIO_PIN_9)
      .init();
  HAL_TIM_Base_Start_IT(&htim7);//定时器代替任务
//  Key_scan_TimerHandle = osTimerNew(Key_scan_Timer_Callback, osTimerPeriodic,
//                                    NULL, &Key_scan_Timer_attributes);
//  osTimerStart(Key_scan_TimerHandle, 10);
}

void Key_scan_Timer_Callback(void *argument) { keypad.Scan_callback_handler(); }