#include <stdio.h>
#include "key.h"
#include "led.h"
#include "delay.h"
#include "ws2812.h"


volatile u8 Key1_value = 0;
volatile u8 Key2_value = 0;
volatile u8 Key3_value = 0;

//按键初始化函数
void KEY_Init(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;
    EXTI_InitTypeDef   EXTI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOC时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOD时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; // 按键 对应引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOD1，4

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8; // 按键 对应引脚
    GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOD1，4

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource9);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource8);
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource11);

    EXTI_InitStructure.EXTI_Line = EXTI_Line9 | EXTI_Line8 | EXTI_Line11;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


void EXTI9_5_IRQHandler(void)
{

    if(EXTI_GetITStatus(EXTI_Line8)==1)
    {
        delay_ms(5);
        if (KEY2 == 0) {
            Key2_value = 1;
        }
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
    else if(EXTI_GetITStatus(EXTI_Line9)==1)
    {
        delay_ms(5);
        if (KEY3 == 0) {
            Key3_value = 1;
        }
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}

void EXTI15_10_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line11)==1)
    {
        delay_ms(5);
        if (KEY1 == 0) {
            Key1_value = 1;
        }
        EXTI_ClearITPendingBit(EXTI_Line11);
    }

}

