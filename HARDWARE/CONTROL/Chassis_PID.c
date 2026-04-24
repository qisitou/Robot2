#include "headfile.h"                  // Device header


PID_PositionInitTypedef Chassis_AnglePID;//底盘转向PID
PID_PositionInitTypedef Avoid_PID;

/*
 *函数简介:底盘PID初始化
 *参数说明:无
 *返回类型:无
 *备注:无
 */
void Chassis_PID_Init(void)
{
	/*===============底盘转向PID===============*/
	float Chassis_AnglePID_Kp=2.0f;			//Kp
	float Chassis_AnglePID_Ki=0.0f;			//Ki
	float Chassis_AnglePID_Kd=3.0f;			//Kd
	float Chassis_AnglePID_EkRange=0.5f;	//误差死区
	float Chassis_AnglePID_OUTRange=200.0f;	//输出限幅

	PID_PositionStructureInit(&Chassis_AnglePID,0);
	PID_PositionSetParameter(&Chassis_AnglePID,Chassis_AnglePID_Kp,Chassis_AnglePID_Ki,Chassis_AnglePID_Kd);
	PID_PositionSetEkRange(&Chassis_AnglePID,-Chassis_AnglePID_EkRange,Chassis_AnglePID_EkRange);
	PID_PositionSetOUTRange(&Chassis_AnglePID,-Chassis_AnglePID_OUTRange,Chassis_AnglePID_OUTRange);

	/*===============避障PID===============*/
	float Avoid_PID_Kp=0.5f;			//Kp
	float Avoid_PID_Ki=0.0f;			//Ki
	float Avoid_PID_Kd=0.0f;			//Kd
	float Avoid_PID_EkRange=0.5f;		//误差死区
	float Avoid_PID_OUTRange=50.0f;	//输出限幅
	PID_PositionStructureInit(&Avoid_PID,avoid_data.dis_target);
	PID_PositionSetParameter(&Avoid_PID,Avoid_PID_Kp,Avoid_PID_Ki,Avoid_PID_Kd);
	PID_PositionSetEkRange(&Avoid_PID,-Avoid_PID_EkRange,Avoid_PID_EkRange);
	PID_PositionSetOUTRange(&Avoid_PID,-Avoid_PID_OUTRange,Avoid_PID_OUTRange);
}
