#include "control.h"
#include "delay.h"
#include "im948_CMD.h"
#include "movement.h"
#include "Emm_V5.h"
#include "sys.h"



u8 Run_Mode = 1;
/*******************************************************************************************************************************************************************************/



float Target_gyroz;


float	Target_Position_LF;
float	Target_Position_RF;
float	Target_Position_LB;
float	Target_Position_RB;


float Target_LB;
float Target_LF;
float Target_RF;
float Target_RB;

float	Motor_PWM_LF;
float	Motor_PWM_RF;
float	Motor_PWM_LB;
float	Motor_PWM_RB;


float PWM_LF;
float PWM_RF;
float PWM_LB;
float PWM_RB;


float Position_P_Yaw=5;
float Position_I_Yaw=0;
float Position_D_Yaw=8;



void Kinematic_Analysis_Speed(float Vx,float Vy,float Wz)
{
   Target_LF = Vy + Vx + Wz*(a_PARAMETER+b_PARAMETER);
   Target_RF = Vy - Vx - Wz*(a_PARAMETER+b_PARAMETER);
   Target_LB = Vy - Vx + Wz*(a_PARAMETER+b_PARAMETER);
   Target_RB = Vy + Vx - Wz*(a_PARAMETER+b_PARAMETER);
}


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
	Bias = Angle-Target;


	if(Bias > 180)Bias -= 360;
		if(Bias < -180)Bias += 360;

	float d_filter_alpha = 0.1;
	static float filtered_deriv = 0;


	float current_deriv = Bias - Last_Bias;
	filtered_deriv = d_filter_alpha * current_deriv + (1 - d_filter_alpha) * filtered_deriv;

//	if(Integral_bias>100000)Integral_bias=100000;
//	if(Integral_bias<-100000)Integral_bias=-100000;
	Pwm=Position_P_Yaw*Bias+Position_I_Yaw*Integral_bias+Position_D_Yaw * filtered_deriv;
	Last_Bias=Bias;


	return Pwm;
}




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


uint16_t ace = 0;
float pwm_4000 = 4;


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
	Emm_V5_Synchronous_motion(0);
	delay_ms(5);
}


/*******************************************************************************************************************************************************************************/

void TIM6_DAC_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)==SET)
	{

		if(Run_Mode == 1)
		{
			Kinematic_Analysis_Speed(Target_Vx_Speed,Target_Vy_Speed,Target_W_Speed);
			Motor_PWM_LF = Target_LF;	Motor_PWM_RF =  Target_RF;
			Motor_PWM_LB = Target_LB;	Motor_PWM_RB =  Target_RB;
		}

		else if(Run_Mode == 2)
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

		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
	}
}

