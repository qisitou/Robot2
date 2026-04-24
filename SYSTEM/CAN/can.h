#ifndef __CAN_H
#define __CAN_H

#include <stdbool.h>
#include "stm32f4xx.h"   

#define CAN_RX  0	// 1:使能CAN接收中断，0:禁止CAN接收中断


typedef struct {
	__IO CanRxMsg CAN_RxMsg;
	__IO CanTxMsg CAN_TxMsg;

	__IO bool rxFrameFlag;
}CAN_t;

void can_init(void);
void can_SendCmd(__IO uint8_t *cmd, uint8_t len);

extern __IO CAN_t can;
extern uint8_t Motor_Num;

#endif
