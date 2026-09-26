#ifndef INC_24LTCK_CX522_H
#define INC_24LTCK_CX522_H

#include "sys.h"


#define CX522_IRQHandler DMA1_Stream2_IRQHandler
#define CX522_UART UART4
#define CX522_DMA_STREAMx DMA1_Stream2

extern u8 cx522_allow;
extern u8 cx522_rxbuf[23];


void cx522_Init(void);
void cx522_ProcessData(void);
void cx522_poll(void);

#endif 
