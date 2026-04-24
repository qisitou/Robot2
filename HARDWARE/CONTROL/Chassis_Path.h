#ifndef __CHASSIS_PATH_H
#define __CHASSIS_PATH_H

#include <stdint.h>

typedef struct
{
	float vx;			//横向速度								向右为正
	float vy;			//纵向速度								向前为正
	float Delta_Angle;	//Yaw旋转角度							逆时针为正
	float t;			//匀速时间								理论上匀速时间为2ms*t
	float K;			//上段路径点到这次路径点的加减速时间	理论上加减速时间为2ms*K
}Chassis_PathPoint;//底盘路径点结构体

typedef struct
{
	float End_K;				//最后一段路径点到停止的加减速时间		理论上加减速时间为2ms*K
	
	uint8_t Path_Size;			//路径点个数
	Chassis_PathPoint *Path;	//路径点数组
}Chassis_Path;//底盘路径结构体

typedef enum
{
	Straight =0,	//直行
	TurnRight=-90,	//右转
	TurnLeft =90,	//左转
	TurnRight_Around =-180,
	TurnLeft_Around =180,
}Chassis_YawTurn;//底盘Yaw旋转

extern Chassis_Path Chassis_Path_StartToTurntable;				//出发到大转盘
extern Chassis_Path Chassis_Path_StartToStairs;					//出发到楼梯
extern Chassis_Path Chassis_Path_StartToWarehouse;				//出发到仓库
extern Chassis_Path Chassis_Path_StartToHome;					//回家



void Chassis_PathInit(void);	//底盘路径初始化

#endif
