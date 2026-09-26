#ifndef __LTCK_H
#define __LTCK_H

#include "sys.h"
typedef enum
{
  None=0,	//未选择
  Red,    //红方
  Blue,   //蓝方
}Color;// 


extern volatile uint8_t turntable_task;
extern volatile uint8_t turnplate_flag;
extern volatile uint8_t ball_num;
extern volatile uint8_t turntabel_end;
extern volatile uint8_t delay_task;
extern volatile int8_t red_or_blue;
extern volatile uint8_t left_warehourse;

void LTCK_Init(void);
void Choose_Color(void);
void Go_To_Turntable(void);
void Go_To_Stairs(void);
void Go_To_Small_Turntable(void);
void Go_To_Warehouse(void);
void Go_To_Home(void);
void Go_To_Turntable___test(void);
void Set_KeepDistance_X(float Vx, float Vy, float angle, float target_angle, float target_dis);

void change_flag(u8 *target_flag,u8 flag);   //延时任务里改标志用的函数（定义在LTCK.c）

#endif

