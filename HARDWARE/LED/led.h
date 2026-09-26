#ifndef __LED_H
#define __LED_H
#include "sys.h"

#define LED_ON      GPIO_ResetBits(GPIOD,GPIO_Pin_4)
#define LED_OFF     GPIO_SetBits(GPIOD,GPIO_Pin_4)

////LED端口定义
//#define LED1(flag) GPIO_WriteBit(GPIOE,GPIO_Pin_1,flag)
#define LED1 PDout(4)
#define LED2 PEout(2)
#define LED3 PEout(3)

#define RUN_LED_GPIO GPIOD
#define RUN_LED_PIN GPIO_Pin_3
#define INDICATE_LED_GPIO GPIOD
#define INDICATE_LED_PIN GPIO_Pin_4
//#define LED_ON GPIO_ResetBits(GPIOE,GPIO_Pin_7 | GPIO_Pin_8);//调试用

extern u8 Indicate_Led_flag;

void LED_Init(void);//初始化
void LED_DeInit(void);
extern void led_set(u8 sta);
void Indicate_Led_Close(void);
#endif






