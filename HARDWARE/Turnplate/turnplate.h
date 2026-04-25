#ifndef TURNPLATE_H
#define TURNPLATE_H

#include "sys.h"

#define TURNPLATE_SERVO_ID 7

#define TURNPLATE_HOLE_NUM 10

struct Hole
{
    char ball;    //0 无球，r 红球，b 篮球, w 未识别    11 第一行倒跺，22 第二行倒跺，33 第三行倒跺
    u8 ic;        //0 未识别
    u16 pos;
};



extern u8 shake_flag;

extern u16 wait_time;
extern int idx_bias;

extern struct Hole HoleArr[10];

extern int8_t Hole_Idx;
extern int8_t Hole_Now_Idx;

void Turnplate_SetPos(u16 pos);
void Turnplate_Move(int8_t idx);

void turnplate_blank(void);
void turnplate_daoduo(u8 row);
void turnplate_log(void);


#endif 
