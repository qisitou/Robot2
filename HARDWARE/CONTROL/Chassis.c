#include "headfile.h"

#define Chassis_LeftFrontWheel		1//左前轮
#define Chassis_RightFrontWheel		2//右前轮
#define Chassis_LeftRearWheel		3//左后轮
#define Chassis_RightRearWheel		4//右后轮


/*
 *函数简介:底盘逆运动学解算控制
 *参数说明:Vx		横向速度	向右为正
 *参数说明:Vy		纵向速度	向前为正
 *参数说明:Wz		旋转速度	逆时针为正
 *返回类型:无
 *备注:无
 */
void Chassis_InverseMotionControl(float v_x,float v_y,float w)
{
	int32_t LeftFront = v_x+v_y-w*(a_PARAMETER+b_PARAMETER);	//左前轮
	int32_t RightFront= v_x-v_y-w*(a_PARAMETER+b_PARAMETER);	//右前轮
	int32_t LeftRear  =-v_x+v_y-w*(a_PARAMETER+b_PARAMETER);	//左后轮
	int32_t RightRear =-v_x-v_y-w*(a_PARAMETER+b_PARAMETER);	//右后轮
	
	StepMotor_SetSpeed(Chassis_LeftFrontWheel,LeftFront);
	StepMotor_SetSpeed(Chassis_RightFrontWheel,RightFront);
	StepMotor_SetSpeed(Chassis_LeftRearWheel,LeftRear);
	StepMotor_SetSpeed(Chassis_RightRearWheel,RightRear);
}

/*
 *函数简介:底盘速度设置
 *参数说明:vx		前进方向横向速度			向右为正
 *参数说明:vy		前进方向纵向速度			向前为正
 *参数说明:theta	前进方向和底盘方向夹角		从前进方向指向底盘方向,逆时针为正
 *返回类型:无
 *备注:进行世界坐标系和底盘坐标系的坐标变换
 */
void Chassis_SetSpeed(float vx,float vy,float Yaw,float Start_Angle)
{
	float theta = Yaw-Start_Angle;

	theta=theta*Data_Deg2Rad;
	
	float vx_=vx*cosf(theta)+vy*sinf(theta);
	float vy_=-vx*sinf(theta)+vy*cosf(theta);
	
	PID_AngleCalc(&Chassis_AnglePID,Yaw);
	Chassis_InverseMotionControl(vx_,vy_,Chassis_AnglePID.OUT);
}


/*
 *函数简介:底盘正弦加减速
 *参数说明:vx1			初始横向速度		向右为正
 *参数说明:vy1			初始纵向速度		向前为正
 *参数说明:vx2			目标横向速度		向右为正
 *参数说明:vy2			目标纵向速度		向前为正
 *参数说明:Angle		前进方向
 *参数说明:K			加减速时间			理论上加减速时间为2ms*K
 *返回类型:无
 *备注:无
 */
void Chassis_SINAccel(float vx1,float vy1,float vx2,float vy2,float Angle,float K)
{
	for(uint16_t i=0;i<K;i++)
	{
		Chassis_SetSpeed(vx1+(vx2-vx1)*(0.5f-0.5f*cosf(PI/K*i)),vy1+(vy2-vy1)*(0.5f-0.5f*cosf(PI/K*i)),Yaw_Angle,Angle);
		delay_ms(2);
	}
}

/*
 *函数简介:底盘匀速运动
 *参数说明:vx			初始横向速度		向右为正
 *参数说明:vy			初始纵向速度		向前为正
 *参数说明:Angle		前进方向
 *参数说明:K			匀速时间			
 *返回类型:无
 *备注:无
 */
void Chassis_FixSpeed(float vx,float vy,float Angle,float K)
{
	for(uint16_t i=0;i<K;i++)
	{
		Chassis_SetSpeed(vx,vy,Yaw_Angle,Angle);
		delay_ms(2);
	}
}

/*
 *函数简介:底盘归位
 *参数说明:无
 *返回类型:无
 *备注:用于移动结束后的稳定Yaw姿态
 */
void Chassis_GuiWei(float Start_Angle , uint16_t time)
{
    float SaveKp=Chassis_AnglePID.Kp,SaveKi=Chassis_AnglePID.Ki,SaveKd=Chassis_AnglePID.Kd;
    float SaveOutLow = Chassis_AnglePID.OUT_low, SaveOutUp = Chassis_AnglePID.OUT_up;

    PID_PositionSetParameter(&Chassis_AnglePID,60,0,3);
    PID_PositionSetOUTRange(&Chassis_AnglePID, -350, 350);   // 临时放开限幅,归位更猛
    for(int i=0;i<time;i++)
    {
        Chassis_SetSpeed(0,0,Yaw_Angle,Start_Angle);
        delay_ms(2);
    }
    PID_PositionSetOUTRange(&Chassis_AnglePID, SaveOutLow, SaveOutUp);   // 恢复限幅
    PID_PositionSetParameter(&Chassis_AnglePID,SaveKp,SaveKi,SaveKd);    // 恢复PID
}


/*
 *函数简介:底盘单次移动
 *参数说明:vx				横向速度		向右为正
 *参数说明:vy				纵向速度		向前为正
 *参数说明:Delta_Angle		Yaw角度			逆时针为正
 *参数说明:t				匀速时间		理论上匀速时间为2ms*t
 *参数说明:K				加减速时间		理论上加减速时间为2ms*K
 *返回类型:无
 *备注:无
 */
void Chassis_MoveOnce(float vx,float vy,float Delta_Angle,float t,float K)
{
	//确定前进方向
	float Start_Angle=Chassis_AnglePID.Need_Value;
	Chassis_AnglePID.Need_Value+=Delta_Angle;//Yaw角度期望
	
	/*===============加速段===============*/
	Chassis_SINAccel(0,0,vx,vy,Start_Angle,K);//采用正弦加减速
	
	/*===============匀速段===============*/
	for(uint16_t i=0;i<t;i++)
	{
		Chassis_SetSpeed(vx,vy,Yaw_Angle,Start_Angle);
		delay_ms(2);
	}
	
	/*===============减速段===============*/
	Chassis_SINAccel(vx,vy,0,0,Start_Angle,K);//采用正弦加减速

	//Chassis_GuiWei();
	Chassis_InverseMotionControl(0,0,0);
}

/*
 *函数简介:底盘单次路径移动
 *参数说明:Path			路径结构体
 *返回类型:无
 *备注:只单次路径移动,无需配置Chassis_Path的End_K
 */
void Chassis_MoveOncePath(Chassis_Path Path)
{
	for(uint8_t i=0;i<Path.Path_Size;i++)
	{
		Chassis_MoveOnce(Path.Path[i].vx,Path.Path[i].vy,0,Path.Path[i].t,Path.Path[i].K);
	}
}

/*
 *函数简介:底盘单路径移动
 *参数说明:Path			路径结构体
 *返回类型:无
 *备注:只移动单个路径,无需配置Chassis_Path的End_K和Path_Size
 */
void Chassis_Move(Chassis_Path Path)
{
	Chassis_MoveOnce(Path.Path[0].vx,Path.Path[0].vy,Path.Path[0].Delta_Angle,Path.Path[0].t,Path.Path[0].K);
}

/*
 *函数简介:底盘路径移动
 *参数说明:Path			路径结构体
 *返回类型:无
 *备注:无
 */
void Chassis_MovePath(Chassis_Path Path)
{
	//确定第一段路径前进方向
	float Path0_Start_Angle=Chassis_AnglePID.Need_Value;
	Chassis_AnglePID.Need_Value+=Path.Path[0].Delta_Angle;//第一段路径Yaw角度期望

	/*===============第一段加速段===============*/
	Chassis_SINAccel(0,0,Path.Path[0].vx,Path.Path[0].vy,Path0_Start_Angle,Path.Path[0].K);//采用正弦加减速
	
	/*===============第一段匀速段===============*/
	for(uint16_t j=0;j<Path.Path[0].t;j++)
	{
		Chassis_SetSpeed(Path.Path[0].vx,Path.Path[0].vy,Yaw_Angle,Path0_Start_Angle);
		delay_ms(2);
	}
	
	/*===============第二段到倒数第二段(i=1~Path_Size-1)===============*/
	for(uint8_t i=1;i<Path.Path_Size-1;i++)
	{
		//确定第i段路径前进方向
		float Start_Angle=Chassis_AnglePID.Need_Value;
		Chassis_AnglePID.Need_Value+=Path.Path[i].Delta_Angle;//第i段路径Yaw角度期望
		
		float vx1=Path.Path[i-1].vx,vy1=Path.Path[i-1].vy;
		float vx2=Path.Path[i].vx,vy2=Path.Path[i].vy;
		
		float theta=Path.Path[i-1].Delta_Angle*Data_Deg2Rad;
		float vx1_=vx1*cosf(theta)+vy1*sinf(theta);
		float vy1_=-vx1*sinf(theta)+vy1*cosf(theta);
		
		/*===============第i-1段到第i段的过渡态===============*/
		Chassis_SINAccel(vx1_,vy1_,vx2,vy2,Start_Angle,Path.Path[i].K);//采用正弦加减速
		
		/*===============第i段匀速段===============*/
		for(uint16_t j=0;j<Path.Path[i].t;j++)
		{
			Chassis_SetSpeed(vx2,vy2,Yaw_Angle,Start_Angle);
			delay_ms(2);
		}
	}
	
	//确定最后一段路径前进方向
	float PathEnd_Start_Angle=Chassis_AnglePID.Need_Value;
	Chassis_AnglePID.Need_Value+=Path.Path[Path.Path_Size-1].Delta_Angle;//最后一段路径Yaw角度期望					
		
	float vx1=Path.Path[Path.Path_Size-2].vx,vy1=Path.Path[Path.Path_Size-2].vy;
	float vx2=Path.Path[Path.Path_Size-1].vx,vy2=Path.Path[Path.Path_Size-1].vy;

	float theta=Path.Path[Path.Path_Size-2].Delta_Angle*Data_Deg2Rad;
	float vx1_=vx1*cosf(theta)+vy1*sinf(theta);
	float vy1_=-vx1*sinf(theta)+vy1*cosf(theta);
	
	/*===============倒数第二段到最后一段的过渡态===============*/
	Chassis_SINAccel(vx1_,vy1_,vx2,vy2,PathEnd_Start_Angle,Path.Path[Path.Path_Size-1].K);//采用正弦加减速
	
	/*===============最后一段匀速段===============*/
	for(uint16_t j=0;j<Path.Path[Path.Path_Size-1].t;j++)
	{
		Chassis_SetSpeed(vx2,vy2,Yaw_Angle,PathEnd_Start_Angle);
		delay_ms(2);
	}
	
	/*===============最后一段减速段===============*/
	Chassis_SINAccel(vx2,vy2,0,0,PathEnd_Start_Angle,Path.End_K);//采用正弦加减速

	//Chassis_GuiWei();
	Chassis_InverseMotionControl(0,0,0);
}


/*
 *函数简介:底盘右转
 *参数说明:无
 *返回类型:无
 *备注:无
 */
void Chassis_TurnRight(void)
{
	Chassis_AnglePID.Need_Value-=180;
	float SaveKp=Chassis_AnglePID.Kp,SaveKi=Chassis_AnglePID.Ki,SaveKd=Chassis_AnglePID.Kd;
	PID_PositionSetParameter(&Chassis_AnglePID,8,0,3);
	while(1)
	{
		PID_AngleCalc(&Chassis_AnglePID,Yaw_Angle);
		if(Chassis_AnglePID.Ek==0)break;
		
		Chassis_InverseMotionControl(0,0,Chassis_AnglePID.OUT*3);
		delay_ms(2);
	}
	PID_PositionSetParameter(&Chassis_AnglePID,SaveKp,SaveKi,SaveKd);//恢复PID
	Chassis_InverseMotionControl(0,0,0);
}


/*
 *函数简介:底盘左转
 *参数说明:无
 *返回类型:无
 *备注:无
 */
void Chassis_TurnLeft(float yaw)
{
	Chassis_AnglePID.Need_Value+=yaw;
	float SaveKp=Chassis_AnglePID.Kp,SaveKi=Chassis_AnglePID.Ki,SaveKd=Chassis_AnglePID.Kd;
	PID_PositionSetParameter(&Chassis_AnglePID,10,0,4);

	while(1)
	{
		PID_AngleCalc(&Chassis_AnglePID,Yaw_Angle);
		if(Chassis_AnglePID.Ek==0)break;
		
		Chassis_InverseMotionControl(0,0,Chassis_AnglePID.OUT*4);
		delay_ms(2);
	}
	PID_PositionSetParameter(&Chassis_AnglePID,SaveKp,SaveKi,SaveKd);//恢复PID
	Chassis_InverseMotionControl(0,0,0);
}
/*
 *函数简介:底盘停止
 *参数说明:无
 *返回类型:无
 *备注:无
 */
void Chassis_Stop(void)
{
	Chassis_InverseMotionControl(0,0,0);
}

