#ifndef __USART6_H
#define __USART6_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

	
#define EN_USART6_RX 			1		//使能（1）/禁止（0）串口6接收

#define readOnly(x)	x->CR1 |= 4;	x->CR1 &= 0xFFFFFFF7;		//串口x配置为只读，CR1->RE=1, CR1->TE=0
#define sendOnly(x)	x->CR1 |= 8;	x->CR1 &= 0xFFFFFFFB;		//串口x配置为只写，CR1->RE=0, CR1->TE=1

//如果想串口中断接收，请不要注释以下宏定义

void usart6_init(u32 bound);
void uart6_WriteBuf(uint8_t *buf, uint8_t len);

#endif


