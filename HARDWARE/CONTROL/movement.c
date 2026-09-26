/*小车的一些基本动作*/

#include "movement.h"
#include "delay.h"
#include "sys.h"
#include "control.h"
#include <string.h>
#include "delay.h"
#include <stdarg.h>
#include <math.h>
#include "Emm_V5.h"
#include "can.h"
#include "headfile.h"


float Target_Vx_Speed;  		//小车的右移目标速度
float Target_Vy_Speed;  		//小车的前进目标速度
float Target_W_Speed;  			//小车的旋转目标角速度
float Target_W_Bias;
float Position_W_Bias;
float Target_Angle;


float Target_Position_Vx;  		//小车的左右移目标位移
float Target_Position_Vy;  		//小车的前进目标位移
float Target_Position_W;  		//小车的旋转目标角位移

extern float Target_gyroz;

extern u8 Run_Mode;
extern int	Position_LF;
extern int	Position_RF;
extern int	Position_LB;
extern int	Position_RB;


uint32_t out_time = 0;

#define MOTOR_PULSE_PER_REV   3200.0f
#define TURN_DIR_LEFT         0
#define TURN_DIR_RIGHT        1
#define TURN_SPEED_MAX_RPM    5000U

/*车子转少了，加大参数  车子转多了，减小参数*/
#define TURN_PULSE_SCALE_LEFT  0.97f
#define TURN_PULSE_SCALE_RIGHT 1.000f

static uint16_t calc_turn_pulse_by_wheel_distance(float wheel_distance_m, char turn_dir)
{
	float pulse_f;
	float scale;
	uint32_t pulse_u32;

	if (turn_dir == TURN_DIR_LEFT)
	{
		scale = TURN_PULSE_SCALE_LEFT;
	}
	else
	{
		scale = TURN_PULSE_SCALE_RIGHT;
	}

	pulse_f = fabsf(wheel_distance_m) * MOTOR_PULSE_PER_REV * scale / (PI * R_PARAMETER);
	pulse_u32 = (uint32_t)(pulse_f + 0.5f);	//减少系统性“少给脉冲”的误差。
	if (pulse_u32 > 65535U)
	{
		pulse_u32 = 65535U;
	}
	return (uint16_t)pulse_u32;
}

static uint32_t calc_turn_wait_ms(uint16_t pulse, uint16_t speed_rpm)
{
	uint32_t denom;
	uint64_t numer;

	if (speed_rpm > TURN_SPEED_MAX_RPM)
	{
		speed_rpm = TURN_SPEED_MAX_RPM;
	}

	denom = (uint32_t)speed_rpm * (uint32_t)MOTOR_PULSE_PER_REV;
	numer = (uint64_t)pulse * 60000ULL;

	/* 向上取整，确保刚到目标角度 */
	return (uint32_t)((numer + (uint64_t)denom - 1ULL) / (uint64_t)denom);
}

void Turn_car_angle(uint16_t speed_rpm,int16_t angle_deg)
{
	char turn_dir;
	char dir_1;
	char dir_2;
	char dir_3;
	char dir_4;
	float yaw_rad;
	uint16_t pulse_1;
	uint16_t pulse_2;
	uint16_t pulse_3;
	uint16_t pulse_4;
	uint16_t pulse_max;
	uint32_t wait_ms;

	if (angle_deg > 0)
	{
		turn_dir = TURN_DIR_RIGHT;
	}
	else
	{
		turn_dir = TURN_DIR_LEFT;
	}

	if (speed_rpm > TURN_SPEED_MAX_RPM)
	{
		speed_rpm = TURN_SPEED_MAX_RPM;
	}
	if (speed_rpm == 0U)
	{
		return;
	}

	yaw_rad = ((float)angle_deg) * PI / 180.0f;
	Kinematic_Analysis_Position(0.0f, 0.0f, yaw_rad);

	/* 电机映射：1->LB, 2->RB, 3->LF, 4->RF */
	dir_1 = (Target_Position_LB >= 0.0f) ? CW : CCW;
	dir_2 = (Target_Position_RB >= 0.0f) ? CCW : CW;
	dir_3 = (Target_Position_LF >= 0.0f) ? CW : CCW;
	dir_4 = (Target_Position_RF >= 0.0f) ? CCW : CW;

	pulse_1 = calc_turn_pulse_by_wheel_distance(Target_Position_LB, turn_dir);
	pulse_2 = calc_turn_pulse_by_wheel_distance(Target_Position_RB, turn_dir);
	pulse_3 = calc_turn_pulse_by_wheel_distance(Target_Position_LF, turn_dir);
	pulse_4 = calc_turn_pulse_by_wheel_distance(Target_Position_RF, turn_dir);

	pulse_max = pulse_1;
	if (pulse_2 > pulse_max) pulse_max = pulse_2;
	if (pulse_3 > pulse_max) pulse_max = pulse_3;
	if (pulse_4 > pulse_max) pulse_max = pulse_4;

	Emm_V5_Pos_Control(1,dir_1,speed_rpm,0,pulse_1,0,1);delay_ms(10);
	Emm_V5_Pos_Control(2,dir_2,speed_rpm,0,pulse_2,0,1);delay_ms(10);
	Emm_V5_Pos_Control(3,dir_3,speed_rpm,0,pulse_3,0,1);delay_ms(10);
	Emm_V5_Pos_Control(4,dir_4,speed_rpm,0,pulse_4,0,1);delay_ms(10);
	Emm_V5_Synchronous_motion(0);

	wait_ms = calc_turn_wait_ms(pulse_max, speed_rpm);
	delay_ms(wait_ms);
}


/* 直接使能电机停车 */
void Stop_now(void)
{	//参数：（电机地址，开启同步）
	Emm_V5_Stop_Now(1,1);delay_ms(10);
	Emm_V5_Stop_Now(2,1);delay_ms(10);
	Emm_V5_Stop_Now(3,1);delay_ms(10);
	Emm_V5_Stop_Now(4,1);delay_ms(10);
	Emm_V5_Synchronous_motion(0); 								 // 广播地址0触发
	delay_ms(10);
}


/* 停车 */
void Stop(void)
{
	out_time = 0;
	while(out_time <  100)
	{
		Run_Mode = 1;
		Target_Vy_Speed = 0;
		Target_W_Speed = 0;
		Target_Vx_Speed = 0;
		Set_Pwm();
	}
}


//运动模式1
void Move_Mode1(float X,float Y,float W,float time)
{
	Run_Mode = 1;
	Target_Vx_Speed = X;
	Target_Vy_Speed = Y;
	Target_W_Speed  = W;
	Set_Pwm();
	delay_ms(time*1000);
}

//运动模式2
void Move_Mode2(float X,float Y,float W,float Angle,float time)
{
	Run_Mode=2;
	Target_Vx_Speed = X;
	Target_Vy_Speed = Y;
	Target_W_Speed  = W;
	Target_gyroz    = Angle;
	Set_Pwm();
	delay_ms(time*1000);
}

