#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"


extern u8 Run_Mode;
extern float Target_W_Bias;



/* 轮胎转动的相关参数 */
#define R_PARAMETER  0.0762f   //0.064				//轮子的直径（单位：m）
//											#define a_PARAMETER  (0.079f)  		//车中心到轮子中心的距离（单位：m）（f表示该常数用浮点方式存储，相当于float）               
#define a_PARAMETER  (0.125f)//(0.085f)  		//3车中心到轮子中心的距离（单位：m）（f表示该常数用浮点方式存储，相当于float）               
#define b_PARAMETER  (0.125f)//(0.080f)  		//车中心到轮子中心的距离（单位：m）（f表示该常数用浮点方式存储，相当于float）      

#define CW   0
#define CCW  1
#define SYNC 1


extern	float Target_LB;		//左后目标速度
extern	float Target_LF;		//左前目标速度
extern	float Target_RF;		//右前目标速度
extern	float Target_RB;		//右后目标速度

extern	float	Target_Position_LF;//===目标位移   
extern	float	Target_Position_RF;      
extern	float	Target_Position_LB;   
extern	float	Target_Position_RB;  



float myabs_f(float a);
void Kinematic_Analysis_Speed(float Vx,float Vy,float Wz);
void Kinematic_Analysis_Position(float Vx,float Vy,float Wz);
void Set_Pwm(void);
int16_t myabs(int16_t a);
float Position_PID_Yaw (float Encoder,float Target);
void TIM6_DAC_IRQHandler(void);  




#endif
