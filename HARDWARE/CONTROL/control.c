#include "control.h"	
#include "delay.h"
#include "im948_CMD.h"
#include "movement.h"
#include "Emm_V5.h"
#include "sys.h"



u8 Run_Mode = 1;
/*******************************************************************************************************************************************************************************/


//陀螺仪原始数据
float Target_gyroz;


float	Target_Position_LF;  //目标位移   
float	Target_Position_RF;      
float	Target_Position_LB;   
float	Target_Position_RB;     


float Target_LB;		//左后目标速度
float Target_LF;		//左前目标速度
float Target_RF;		//右前目标速度
float Target_RB;		//右后目标速度

float	Motor_PWM_LF;	//左前 轮子的PWM
float	Motor_PWM_RF;	//右前 轮子的PWM
float	Motor_PWM_LB;	//左后 轮子的PWM
float	Motor_PWM_RB;	//右后 轮子的PWM	


float PWM_LF;
float PWM_RF;
float PWM_LB;
float PWM_RB;


float Position_P_Yaw=5;
float Position_I_Yaw=0;
float Position_D_Yaw=8;



/**************************************************************************
函数功能：麦轮小车运动数学模型
入口参数：Vy：小车前进速度、Wz:小车旋转角速度（正方向：上，右，顺时针）
返回  值：无
*******************************************************************1*******/
void Kinematic_Analysis_Speed(float Vx,float Vy,float Wz)
{
   Target_LF = Vy + Vx + Wz*(a_PARAMETER+b_PARAMETER); 
   Target_RF = Vy - Vx - Wz*(a_PARAMETER+b_PARAMETER);
   Target_LB = Vy - Vx + Wz*(a_PARAMETER+b_PARAMETER); 
   Target_RB = Vy + Vx - Wz*(a_PARAMETER+b_PARAMETER);  			
}

/**************************************************************************
函数功能：获取位置控制过程速度值
入口参数：X Y Z 三轴位置变化量（正方向：上，右，顺时针）
返回  值：无
**************************************************************************/
void Kinematic_Analysis_Position(float Vx,float Vy,float Vz)
{
	Target_Position_LF = Vy + Vx + Vz*(a_PARAMETER+b_PARAMETER);
	Target_Position_RF = Vy - Vx - Vz*(a_PARAMETER+b_PARAMETER);
	Target_Position_LB = Vy - Vx + Vz*(a_PARAMETER+b_PARAMETER);
	Target_Position_RB = Vy + Vx - Vz*(a_PARAMETER+b_PARAMETER);			
}


static float Position_PID_Yaw (float Angle,float Target)
{ 	
	static float Bias,Pwm,Integral_bias,Last_Bias;
	Bias = Angle-Target; 										//计算偏差

	//使偏差控制在-180到180
	if(Bias > 180)Bias -= 360;
  	if(Bias < -180)Bias += 360;	

	float d_filter_alpha = 0.1;  // 滤波系数（0.1~0.5，越小滤波越强）
	static float filtered_deriv = 0;

	// 修改微分项计算
	float current_deriv = Bias - Last_Bias;
	filtered_deriv = d_filter_alpha * current_deriv + (1 - d_filter_alpha) * filtered_deriv;
//	Integral_bias+=Bias;	                                 //求出偏差的积分
//	if(Integral_bias>100000)Integral_bias=100000;
//	if(Integral_bias<-100000)Integral_bias=-100000;
	Pwm=Position_P_Yaw*Bias+Position_I_Yaw*Integral_bias+Position_D_Yaw * filtered_deriv;       //位置式PID控制器
	Last_Bias=Bias;                                       //保存上一次偏差 

	
	return Pwm;                                          //增量输出
}



/**************************************************************************
函数功能：绝对值函数
入口参数：u16
返回  值：unsigned int 16
**************************************************************************/
int16_t myabs(int16_t a)
{ 		   
	  int16_t temp;
	  if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}

float myabs_f(float a)
{ 		   
	  float temp_f;
	  if(a<0)  temp_f = -a;  
	  else temp_f = a;
	  return temp_f;
}


uint16_t ace = 0;//加速度，直接给0，不加减速
float pwm_4000 = 4;//速度倍数

/*
1、在 CAN 接收中断里面时，第一个电机收到同步帧的时间没有太长，但是错过了这个周期的上升沿，要等待下一个脉冲过来，表现为“偶尔停一下”。
2、但是进中断太久，第一个电机等待时间过久，接受同步帧超时，就删除了此次内部寄存器参数的设置，导致电机停一下。

*/
static void Set_One_Motor_Pwm(u8 id, float motor_pwm, float *pwm_cache, u8 dir_positive, u8 dir_negative)
{
	if(motor_pwm > 0)
	{
		*pwm_cache = motor_pwm;
		Emm_V5_Vel_Control(id, dir_positive, (uint16_t)((*pwm_cache) * pwm_4000), ace, SYNC);
	}
	else
	{
		*pwm_cache = myabs_f(motor_pwm);
		Emm_V5_Vel_Control(id, dir_negative, (uint16_t)((*pwm_cache) * pwm_4000), ace, SYNC);
	}
	delay_ms(5);
}


void Set_Pwm(void)
{	
	Set_One_Motor_Pwm(1, Motor_PWM_LB, &PWM_LB, CW, CCW);
	Set_One_Motor_Pwm(3, Motor_PWM_LF, &PWM_LF, CW, CCW);
	Set_One_Motor_Pwm(2, Motor_PWM_RB, &PWM_RB, CCW, CW);
	Set_One_Motor_Pwm(4, Motor_PWM_RF, &PWM_RF, CCW, CW);
	Emm_V5_Synchronous_motion(0); 	// 广播地址0触发
	delay_ms(5);	
}


/*******************************************************************************************************************************************************************************/
//定时器6中断服务函数(10ms进一次，调用运动模型函数)
void TIM6_DAC_IRQHandler(void)  
{	
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)==SET) //溢出中断
	{

		if(Run_Mode == 1)		//不加陀螺仪，直接步进电机裸跑
		{
			Kinematic_Analysis_Speed(Target_Vx_Speed,Target_Vy_Speed,Target_W_Speed); 
			Motor_PWM_LF = Target_LF;	Motor_PWM_RF =  Target_RF;		
			Motor_PWM_LB = Target_LB;	Motor_PWM_RB =  Target_RB;
		}

		else if(Run_Mode == 2)	//加上陀螺仪，位置PID控制
		{

			if(Target_gyroz == 180 && Yaw_Angle < 0 )
				Yaw_Angle= Yaw_Angle+360;
			
			if(Target_gyroz == -180 && Yaw_Angle > 0 )
				Yaw_Angle= Yaw_Angle-360;
			
			Target_W_Bias=Position_PID_Yaw(Yaw_Angle,Target_gyroz);

			if(Target_W_Bias > 500)  Target_W_Bias = 500;
			else if(Target_W_Bias < -500)  Target_W_Bias = -500;

			Kinematic_Analysis_Speed(Target_Vx_Speed,Target_Vy_Speed,Target_W_Speed + Target_W_Bias);	
							
			Motor_PWM_LF = Target_LF;	Motor_PWM_RF =  Target_RF;		
			Motor_PWM_LB = Target_LB;	Motor_PWM_RB =  Target_RB;		
		}	

		TIM_ClearITPendingBit(TIM6,TIM_IT_Update); //清除中断标志位		
	}
}		 

