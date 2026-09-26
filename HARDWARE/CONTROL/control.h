#ifndef __CONTROL_H
#define __CONTROL_H
#include "sys.h"


extern u8 Run_Mode;
extern float Target_W_Bias;



#define R_PARAMETER  0.0762f

#define a_PARAMETER  (0.125f)
#define b_PARAMETER  (0.125f)

#define CW   0
#define CCW  1
#define SYNC 1


extern	float Target_LB;
extern	float Target_LF;
extern	float Target_RF;
extern	float Target_RB;

extern	float	Target_Position_LF;
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
