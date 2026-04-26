#include "headfile.h"

int8_t Hole_Idx = 0;
int8_t Hole_Now_Idx = 0;


int8_t last_Idx = 0;
u16 wait_time = 0;

int idx_bias = 0;

/*
0.5ms ~ 2.5ms 
 500  ~ 2500   (逆时针)
  0   ~ 360度
*/

struct Hole HoleArr[10] = {
        {.pos = 2420},
        {.pos = 2210},
        {.pos = 2020},
        {.pos = 1820},
        {.pos = 1610},
        {.pos = 1410},
        {.pos = 1210},
        {.pos = 1020},
        {.pos = 810},
        {.pos = 600},
};

void Turnplate_SetPos(u16 pos)
{
    TIM_SetCompare1(TIM3,pos);
}

void Turnplate_Move(int8_t idx)
{
    Hole_Now_Idx = idx;

    idx_bias = idx - last_Idx;

    if(idx_bias < 0)
    {
        wait_time = -idx_bias*120+200;
    }
    else
    {
        wait_time = idx_bias*120+200;
    }

    last_Idx = idx;

    Turnplate_SetPos(HoleArr[idx].pos);
}

void turnplate_blank(void)
{
    Hole_Idx = 0;
    while(HoleArr[Hole_Idx].ball != 0) //找到没有球的洞
    {
        Hole_Idx++;
    }

    Turnplate_Move(Hole_Idx);
}

void turnplate_daoduo(u8 row)
{
    int8_t res = 0;

    for(int8_t i = 0; i < 10; i++)
    {
        if((row<<4|row) == HoleArr[i].ball)
        {
            res = i;
        }
    }

    if (res < 3)
    {
        Hole_Idx = res + 7;
    } else
    {
        Hole_Idx = res - 3;
    }
    Turnplate_Move(Hole_Idx);
}

// void shake()	//抖一抖，更好的读到ic卡
// {
//     if(1 == shake_flag)
//     {
//         Turnplate_SetPos(HoleArr[Hole_Idx].pos-22);
//         DelayTask_Add(1,80,(void (*)(void)) change_flag,"%d%d",&shake_flag,2);
//         shake_flag = 0;
//     }
//     else if(2 == shake_flag)
//     {
//         Turnplate_SetPos(HoleArr[Hole_Idx].pos+38);
//         DelayTask_Add(1,80,(void (*)(void)) change_flag,"%d%d",&shake_flag,1);
//         shake_flag = 0;
//     }
// }


void turnplate_log(void)
{
    printf("\r\n<<<\r\n");
    for (int i = 0; i < 10; i++)
    {
        printf("id:%d,ball:%c,ic:%#x\r\n", i, HoleArr[i].ball, HoleArr[i].ic);
    }
    printf(">>>\r\n");
}
