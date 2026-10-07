#ifndef INC_2023GAI_DELAYTASK_H
#define INC_2023GAI_DELAYTASK_H

#include "sys.h"


extern u16  TIME_S ;
typedef struct
{
    u16 Current_DelayTime_ms;
    u16 DelayTime_ms;
    u32 Times;
    void (*FUNC)(void);
    u8 param_num;
    int *params;
    struct DelayTask *next;
}DelayTask;



void DelayTask_Add(u32 Times,u16 Delay_ms, void (*FUNC)(void),char *format, ...);
/* Main-loop only; returns number removed. Matches FUNC and first pointer argument. */
u16 DelayTask_CancelByTarget(void (*FUNC)(void), const volatile void *target);



#endif 
