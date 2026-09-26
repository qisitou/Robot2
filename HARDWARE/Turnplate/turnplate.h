#ifndef TURNPLATE_H
#define TURNPLATE_H

#include "sys.h"

#define TURNPLATE_SERVO_ID 7

#define TURNPLATE_HOLE_NUM 10

struct Hole
{
    u8 ball;    //0rb, w δʶ    1是未识别,0是识别过
    u8 ic;        //0 δʶ
    u16 pos;
};

extern uint8_t Ball_falling(void);

extern u8 shake_flag;

/* 检测状态机相关变量 */
extern u8 detect_allow;
extern u8 detect_stage;
extern u8 detect_flag;
extern u8 detect_cpl;
extern u8 continue_flag;

extern int8_t turnplate_dir;

extern char target_color;
extern char anti_color;

extern u16 timeout_S;

extern u16 wait_time;
extern int idx_bias;

extern struct Hole HoleArr[10];

extern int8_t Hole_Idx;
extern int8_t Hole_Now_Idx;

extern int8_t hole ;

void Turnplate_SetPos(u16 pos);
void Turnplate_Move(int8_t idx);

void turnplate_blank(void);
void turnplate_daoduo(u8 row);
void turnplate_log(void);
// void turnplate_detect(void);
void turnplate_detect_test(void);
void shake(void);

#endif 
