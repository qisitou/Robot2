#include <stdio.h>
#include "led.h"


u8 Indicate_Led_flag = 0;

//LED IO初始化
void LED_Init(void)    //初始化LED引脚
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOD时钟

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化


  //GPIO_SetBits(GPIOD,GPIO_Pin_3);//关闭
  GPIO_ResetBits(GPIOD,GPIO_Pin_3);//打开

}


void LED_DeInit(void)    //LED引脚反初始化
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//使能GPIOG时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
    GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化

}

void Indicate_Led_Close(void)
{
    GPIO_SetBits(INDICATE_LED_GPIO,INDICATE_LED_PIN);
}

void led_set(u8 sta)
{
    if(0 == sta)
    {
        GPIO_SetBits(GPIOD,GPIO_Pin_3);
    }
    else
    {
        GPIO_ResetBits(GPIOD,GPIO_Pin_3);
    }
}
