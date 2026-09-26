#ifndef __KEY_H
#define __KEY_H
#include "sys.h"

///*下面的方式是通过直接操作库函数方式读取IO*/
//#define KEY1 		GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_7) //
//#define KEY2 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_11)	//
/*下面的方式是通过直接操作库函数方式读取IO*/
#define KEY1 		PDin(11)
#define KEY2 		PCin(8)
#define KEY3 		PCin(9)

extern volatile u8 Key1_value;
extern volatile u8 Key2_value;
extern volatile u8 Key3_value;

extern volatile u8 Key1_flag;
extern volatile u8 Key2_flag;
extern volatile u8 Key3_flag;


void KEY_Init(void);	//IO初始化


#endif
