#ifndef __USART2_H
#define __USART2_H
#include "stm32f4xx_conf.h"

	
#define EN_USART2_RX 			1		//使能（1）/禁止（0）串口2接收
	  	


//如果想串口中断接收，请不要注释以下宏定义


extern u16 U2_Rx_Index;
extern u16 U2_Rx_Cpl;
extern u8 U2_Rx_Buf[120];

void usart2_init(u32 bound);
void uart2_WriteBuf(uint8_t *buf, uint8_t len);




#endif


