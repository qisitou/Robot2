#ifndef __BEEP_H
#define __BEEP_H

#include "sys.h"

#define Beep_On			GPIO_ResetBits(GPIOE,GPIO_Pin_8);
#define Beep_Off		GPIO_SetBits(GPIOE,GPIO_Pin_8);

#define BEEP		PFout(9)


void Beep_Init(void);

#endif








