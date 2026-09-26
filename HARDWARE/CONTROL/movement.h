#ifndef __MOVEMENT_H
#define __MOVEMENT_H

#include "sys.h"



extern float Target_Vx_Speed;  		//小车的右移目标速度
extern float Target_Vy_Speed;  		//小车的前进目标速度
extern float Target_W_Speed;  		//小车的旋转目标角速度
extern float Target_Position_Vx;  //小车的左右移目标位移
extern float Target_Position_Vy;  //小车的前进目标位移
extern float Target_Position_W;  	//小车的旋转目标角位移
extern uint32_t out_time;


/* 运动的基本动作 */
void Stop(void);
void Stop_now(void);
void Turn_car_angle(uint16_t speed_rpm,int16_t angle_deg);
void Move_Mode1(float X,float Y,float W,float time);
void Move_Mode2(float X,float Y,float W,float Angle,float time);



#endif












