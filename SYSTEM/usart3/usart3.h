#ifndef __USART3_H
#define __USART3_H
#include "stm32f4xx_conf.h"


#define EN_USART3_RX 			1		//使能（1）/禁止（0）串口2接收
#define EN_PRINTF_USART3 		1		//使能（1）/禁止（0）printf函数

#define USART_REC_LEN  			200  	//定义最大接收字节数 200

extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符
extern u16 USART_RX_STA;         		//接收状态标记

void usart3_init(u32 bound);
void uart3_WriteBuf(uint8_t *buf, uint8_t len);

#endif


