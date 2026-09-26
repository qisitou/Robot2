#ifndef __RING_LIGHT_H
#define __RING_LIGHT_H

#include "stm32f4xx.h"

#define RING_LIGHT_GPIO     GPIOF
#define RING_LIGHT_PIN      GPIO_Pin_12
#define RING_LIGHT_ON_LEVEL Bit_SET

void RingLight_Init(void);
void RingLight_On(void);
void RingLight_Off(void);
void RingLight_Set(uint8_t on);

#endif
