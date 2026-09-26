#ifndef __CHASSIS_H
#define __CHASSIS_H

#include "Chassis_PID.h"
#include "Chassis_Path.h"

#define PI 					 3.14159265f
#define Data_Deg2Rad        0.01745329251994329576923690768489f //PI/180    角度转弧度



void Chassis_Init(void);																					//底盘初始化
void Chassis_MoveOnce(float vx,float vy,float Delta_Angle,float t,float K);									//底盘单次移动
void Chassis_MoveOncePath(Chassis_Path Path);																//底盘单次路径移动
void Chassis_Move(Chassis_Path Path);																		//底盘单路径移动
void Chassis_MovePath(Chassis_Path Path);																	//底盘路径移动													//底盘转到目标角度
void Chassis_TurnLeft(float yaw);																					//底盘左转
void Chassis_InverseMotionControl(float v_x,float v_y,float w);
void Chassis_SetSpeed(float vx,float vy,float Yaw,float Start_Angle);
void Chassis_GuiWei(float Start_Angle , uint16_t time);
void Chassis_SINAccel(float vx1,float vy1,float vx2,float vy2,float Angle,float K);
void Chassis_FixSpeed(float vx,float vy,float Angle,float K);
void Chassis_Stop(void);

#endif
