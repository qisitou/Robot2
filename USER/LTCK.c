#include "headfile.h"

bool Start_init = false;
Color Selected_Color = None;


void LTCK_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
    Gray_Init();
	LED_Init();
	Infrared_Init();	//红外开关
	KEY_Init();
	delay_init(168);
	uart5_init(115200);     //串口打印
	usart3_init(9600);		//舵机控制板
	uart4_init(9600);		//读卡器
	usart2_init(115200);    //陀螺仪
	usart6_init(115200);	//二维码
	IM948_Init();
	can_init();
	TIM5_Int_Init(42-1, 1500-1);        // 1.5ms  进入一次中断	1s-->(10000-1, 8400-1) 
	TIM6_Int_Init(100-1, 8400-1);		  //10ms 进入一次中断
	TIM7_Int_Init(100-1, 8400-1);		  //10ms进入一次中断，延时执行任务 （Delaytask.c）
	PwmServo_Init(20000-1,84-1);		  //初始化舵机 20ms 周期的 PWM 波（50Hz）

	MYDMA_Config(DMA1_Stream3, DMA_Channel_4, (u32)&USART3->DR,(u32)LobotTxBuf,7, 0);				  //DMA1,STEAM3,CH4,外设为串口1,存储器为LobotTxBuf,长度为:SEND_BUF_SIZE.（只是用于发送执行动作组指令）
	MYDMA_Config(DMA1_Stream2, DMA_Channel_4, (u32)&UART4->DR,(u32)cx522_rxbuf, 23, 1);				  //DMA1,STEAM2,CH4,外设为串口4,存储器为cx522_rxbuf,长度为:23（DMA接收读卡器数据）
	MYDMA_Config(DMA1_Stream4, DMA_Channel_0, (u32)&SPI2->DR, (u32)pixelBuffer, Pixel_S1_NUM * 24, 0);//DMA1,STEAM4,CH0,外设为SPI2,存储器为pixelBuffer,长度为:Pixel_S1_NUM * 24（DMA发送WS2812像素数据）

	cx522_Init();
	VL53L0X_All_Init();
	WS2812b_Configuration();
	rgb_SetColor(RGB_1,RED);
	
	XM1603_Init();
	StepMotor_Init();
	Chassis_PathInit();
	Chassis_PID_Init();
	delay_ms(50);
	rgb_SetColor(RGB_1,YELLOW);
}

void Choose_Color(void)
{
	while(!Start_init)
	{
		//printf("start_Yaw:%.1f\n",Yaw_Angle);
		if(Key1_value)		
		{
			Selected_Color++;
			if(Selected_Color > 3)
			{
				Selected_Color = None;
			}

			switch(Selected_Color)
			{	
				case None:
					rgb_SetColor(RGB_2, BLACK);
					break;
				case Red:
					rgb_SetColor(RGB_2, RED);
					break;
				case Blue:
					rgb_SetColor(RGB_2, BLUE);
					break;
			}
			Key1_value = 0;
		}

		if(Key3_value)		//开始比赛，指示灯变绿
		{
			if(Selected_Color == Red || Selected_Color == Blue)
			{
				Start_init = true;
				rgb_SetColor(RGB_4,GREEN);
			}
			Key3_value = 0;
		}
	}
}

void Go_To_Turntable(void)
{
	//前往大转盘
	Chassis_MovePath(Chassis_Path_StartToTurntable);
	//定位
	while(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_3) != 0)
	{
		Chassis_SetSpeed(-50,0,Yaw_Angle,0);
	}
	delay_ms(10); //必须得加一个延时，不然下一步会直接跳过
	Chassis_InverseMotionControl(0,0,0);
	delay_ms(10);
	while(GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_3) == 0)
	{
		Chassis_SetSpeed(0,-50,Yaw_Angle,0);
	}
	Chassis_InverseMotionControl(0,0,0);
	delay_ms(10);
	Chassis_MoveOnce(0,50,0,600,100);
	printf("target1:%f\n",Chassis_AnglePID.Need_Value);
	//Chassis_GuiWei();
	
}	

void Avoid_Obstacle(void)
{
	//避障
	/*
	avoid_data.vx --远离或靠近障碍物
	avoid_data.vy --前进的快慢
	avoid_data.vw --旋转速度
	*/
	while(Yaw_Angle <=82)
	{	
		avoid_data.dis_target=250;  //避障目标距离250mm

		vl53l0x_start_single_test(&vl53l0x_dev4, &vl53l0x_data);
		avoid_data.dis_now = (float) vl53l0x_data.RangeMilliMeter;
		printf("data:%d\n",vl53l0x_data.RangeMilliMeter);
		avoid_data.vx = (avoid_data.dis_now - avoid_data.dis_target)/20;  //简单的比例控制，除以20是为了调节速度
		printf("%f\n",avoid_data.vx);
		if(avoid_data.vx > 30)  avoid_data.vx =  30; //限制最大速度
		if(avoid_data.vx < -30) avoid_data.vx = -30; //限制最小速度

		PID_PositionCalc(&Avoid_PID, avoid_data.dis_now); //计算避障PID，输出到avoid_data.vw
		avoid_data.vw = 200 - Avoid_PID.OUT*3; //基础转向速度为85，根据PID输出调整
		avoid_data.vy = -(avoid_data.vw * 0.6); //转向速度的一半作为横向速度，调整这个比例可以改变转弯的锐利程度

		Chassis_InverseMotionControl(avoid_data.vx, avoid_data.vy, avoid_data.vw);
	}
	printf("finish!\n");
	Chassis_AnglePID.Need_Value+=90;
	printf("target:%f\n",Chassis_AnglePID.Need_Value);
}

void Go_To_Stairs(void)
{
	//前往阶梯

	/*====================前往测量障碍物====================*/
	do
	{
		/* code */
	  vl53l0x_start_single_test(&vl53l0x_dev4,&vl53l0x_data);
	  Chassis_SetSpeed(70,-150,Yaw_Angle,0);		
	} while (vl53l0x_data.RangeMilliMeter < 400);
		
	/*====================避障====================*/
	Avoid_Obstacle();
	/*====================回正====================*/
	Chassis_GuiWei(90);
	/*====================左转====================*/
	Chassis_TurnLeft();	
	/*====================前往测距阶梯====================*/
	do
	{
		/* code */
		vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
		Chassis_SetSpeed(-90,50,Yaw_Angle,180);
	} while (vl53l0x_data.RangeMilliMeter < 400);//400这个值应该大一点，因为小车前面没有物体时，激光测到的数据是不确定性的，会在100~250之间，会误判

	/*====================检测到阶梯====================*/
	do
	{
		/* code */
		vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
		Chassis_SetSpeed(-90,50,Yaw_Angle,180);
	} while (vl53l0x_data.RangeMilliMeter > 200);
	
	/*====================细调与阶梯的距离====================*/ 
	do
	{
		/* code */
		vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
		Chassis_SetSpeed(-30,0,Yaw_Angle,180);
	} while (vl53l0x_data.RangeMilliMeter > 120);
	/*====================回正====================*/
	Chassis_GuiWei(180);
	/*====================定位前准备1:小车移动到离开阶梯的位置====================*/
	do
	{
		/* code */
		vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
		Chassis_SetSpeed(0,-50,Yaw_Angle,180);
	} while (vl53l0x_data.RangeMilliMeter < 122);
	delay_ms(100);
	/*====================定位前准备2:小车回到阶梯的位置====================*/
	do
	{
		/* code */
		vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
		//printf("data:%d\n",vl53l0x_data.RangeMilliMeter);
		Chassis_SetSpeed(0,50,Yaw_Angle,180);
	} while (vl53l0x_data.RangeMilliMeter > 120);
	delay_ms(50);
	while(1)
	{
		Chassis_InverseMotionControl(0,0,0);
	}

	
}

void Go_To_Warehouse(void)
{
	//前往仓库
	Chassis_MovePath(Chassis_Path_StartToWarehouse);
}	
void Go_To_Home(void)
{
	Chassis_TurnLeft();
	//前往家
	Chassis_MovePath(Chassis_Path_StartToHome);
}
