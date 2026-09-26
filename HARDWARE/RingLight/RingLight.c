#include "RingLight.h"

void RingLight_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
    GPIO_InitStructure.GPIO_Pin = RING_LIGHT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(RING_LIGHT_GPIO, &GPIO_InitStructure);

    RingLight_Off();
}

void RingLight_On(void)
{
    GPIO_SetBits(RING_LIGHT_GPIO, RING_LIGHT_PIN);
}

void RingLight_Off(void)
{
    GPIO_ResetBits(RING_LIGHT_GPIO, RING_LIGHT_PIN);
}

void RingLight_Set(uint8_t on)
{
    if (on) RingLight_On();
    else RingLight_Off();
}
