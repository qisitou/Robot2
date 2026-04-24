#include "beep.h"


void Beep_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); //使能GPIOE时钟
  
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//下拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIO
	
	
  GPIO_SetBits(GPIOE,GPIO_Pin_8);//GPIOE8设置高，蜂鸣器不响
	
}






