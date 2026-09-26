#include "headfile.h"

bool Start_init = false;
Color Selected_Color = None;
int flag = 1;


volatile uint8_t turntable_task = 1;
volatile uint8_t turnplate_flag = 1;
volatile uint8_t ball_num = 0;
volatile uint8_t turntabel_end = 0;

volatile uint8_t Stairs_Task=1;

void LTCK_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	/*
		优先级高低：外设 抢占优先级 子优先级

		陀螺仪                0   0
		电机（TIM5）          1   0
		延时函数（TIM7）      1   2
	*/
    Gray_Init();			//灰度传感器
	LED_Init();				//LED
	Infrared_Init();		//红外开关
	KEY_Init();				//按键
	delay_init(168);		//延时函数
	uart5_init(115200);     //串口打印
	usart1_init(230400);    //摄像头通信
	//usart3_init(9600);		//舵机控制板
	uart4_init(9600);		//读卡器
	usart2_init(115200);    //陀螺仪
	IM948_Init();			//陀螺仪初始化
	PwmServo_Init(20000-1,84-1);		//初始化舵机 20ms 周期的 PWM 波（50Hz） --PA6
	delay_ms(2000);			//等待陀螺仪稳定
	can_init();				//CAN总线初始化
	TIM5_Int_Init(42-1, 1500-1);          // 1.5ms 进入一次中断 底盘控制专用
	TIM6_Int_Init(100-1, 8400-1);		  //10ms 进入一次中断
	TIM7_Int_Init(100-1, 8400-1);		  //10ms进入一次中断，延时执行任务 （Delaytask.c）

	MYDMA_Config(DMA1_Stream3, DMA_Channel_4, (u32)&USART3->DR,(u32)LobotTxBuf,7, 0);				  //DMA1,STEAM3,CH4,外设为串口1,存储器为LobotTxBuf,长度为:SEND_BUF_SIZE.（只是用于发送执行动作组指令）
	MYDMA_Config(DMA1_Stream2, DMA_Channel_4, (u32)&UART4->DR,(u32)cx522_rxbuf, 23, 1);				  //DMA1,STEAM2,CH4,外设为串口4,存储器为cx522_rxbuf,长度为:23（DMA接收读卡器数据）
	MYDMA_Config(DMA1_Stream4, DMA_Channel_0, (u32)&SPI2->DR, (u32)pixelBuffer, Pixel_S1_NUM * 24, 0);//DMA1,STEAM4,CH0,外设为SPI2,存储器为pixelBuffer,长度为:Pixel_S1_NUM * 24（DMA发送WS2812像素数据）

	cx522_Init();				//读卡器初始化
	VL53L0X_All_Init();			//激光测距初始化
	WS2812b_Configuration();	//WS2812B像素灯初始化
	rgb_SetColor(RGB_1,RED);
	
	StepMotor_Init();			//步进电机初始化
	Chassis_PathInit();			//底盘路径初始化
	Chassis_PID_Init();			//底盘PID初始化
	delay_ms(50);
	rgb_SetColor(RGB_1,YELLOW);

    DelayTask_Add(100000,10,(void (*)(void)) turnplate_detect_test,0);
}

void Choose_Color(void)
{
	while(!Start_init)
	{
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
	uint8_t Disc_flag = 2;
	//前往大转盘
	DelayTask_Add(1, 1000, (void (*)(void)) runActionGroup, "%d%d%d", 0, 1, false);
	Chassis_MovePath(Chassis_Path_StartToTurntable);
	//定位
	while(Range_ConsecutiveMatch_AutoCnt(GRAY_CH1, CMP_EQ, 0, 3))
	{
		Chassis_SetSpeed(-30,0,Yaw_Angle,0);
	}
	Chassis_Stop();	
	printf("GRAY_CH1:%d\n\r",GRAY_CH1);	
	while(Range_ConsecutiveMatch_AutoCnt((GRAY_CH1 != 0), CMP_EQ, 1, 3))
	{
		Chassis_SetSpeed(0,-70,Yaw_Angle,0);
		delay_ms(20);
	}
	printf("GRAY_CH1_2:%d\n\r",GRAY_CH1);		
	Chassis_MoveOnce(0,50,0,600,100);
	Chassis_GuiWei(0);

	// while(turntabel_end==0)
	// {			
	// 	switch (turntable_task)
	// 	{
	// 	case 1:
	// 		runActionGroup(3,1,false); 
	// 		openmv_send("{1}");
	// 		openmv_send("{S}");  
	// 		DelayTask_Add(1,100,(void (*)(void)) change_flag,"%d%d",&turntable_task,2);
	// 		turntable_task=0;
	// 		break;
	// 	case 2:
	// 		if(turnplate_flag==1 && (Disc_flag != 3))
	// 		{
	// 			if (Ball_falling() == 0)
	// 			{

	// 				HoleArr[Hole_Idx].ball = 0;
	// 				Hole_Idx++;
	// 				ball_num++;

	// 				if(6 == ball_num)
	// 				{
	// 					for(int i=0;i<10;i++)
	// 					{
	// 						printf("id:%d,ball:%d,ic:%#x\r\n",i,HoleArr[i].ball,HoleArr[i].ic);
	// 					}
	// 					turntabel_end =1;
	// 					break;
	// 				}

	// 				Turnplate_Move(Hole_Idx);

	// 				DelayTask_Add(1,500,(void (*)(void)) change_flag,"%d%d",&turnplate_flag,1);
	// 				turnplate_flag = 0;
	// 			}
				
	// 		}		
	// 		if(2 == Disc_flag)
	// 		{
	// 			if(1 == openmv_rx_cpl)
	// 			{
	// 				openmv_rx_command = 0;
	// 				sscanf(openmv_rxbuf,"{%c}",&openmv_rx_command);
	// 	//            printf("openmv_rx_comm:%c",openmv_rx_command);

	// 				if(target_color == openmv_rx_command)
	// 				{
	// 					runActionGroup(4,1,false);
	// 					Disc_flag = 3;
	// 				}
	// 			}
	// 		}
	// 		else if(3 == Disc_flag)
	// 		{
	// 			if(1 == isActionGroupCompleted)
	// 			{
	// 				DelayTask_Add(1,100,(void (*)(void)) change_flag,"%d%d",&Disc_flag,2);
	// 				DelayTask_Add(1,100,(void (*)(void)) change_flag,"%d%d",&openmv_rx_cpl,0);
	// 				Disc_flag = 0;
	// 			}
	// 		}
	// 		break;
	// 	}
	// }	
}	


void Set_KeepDistance_X(float Vx ,float Vy,float angle,float target_angle,float target_dis)
{
	Vx = 0.3 * (target_dis - vl53l0x_data.RangeMilliMeter);
	if(Vx>200)Vx = 0;
	if(Vx<-200)Vx = 0;
	Chassis_SetSpeed(Vx,Vy,angle,target_angle);
}

void Set_KeepDistance_Y(float Vx ,float Vy,float angle,float target_angle,float target_dis)
{
	Vy = 0.3 * (target_dis - vl53l0x_data.RangeMilliMeter);
	if(Vy>200)Vy = 0;
	if(Vy<-200)Vy = 0;
	Chassis_SetSpeed(Vx,Vy,angle,target_angle);
}

void change_flag(u8 *target_flag,u8 flag)
{
    *target_flag = flag;
}


void Go_To_Stairs(void)
{
	u8 task=1;
	//前往阶梯
	while(Yaw_Angle>-60)
	{
		Chassis_InverseMotionControl(300, 0, -300);
	}
	while(Yaw_Angle>-90)
	{
		Chassis_InverseMotionControl(200, 0, -200);
	}
	Chassis_TurnRight();
	do
	{
		/* code */
		vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
		Chassis_SetSpeed(-40,130,Yaw_Angle,180);
	} while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_LT, 400, 3));

	/*====================检测到阶梯====================*/
	do
	{
		/* code */
		vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
		Chassis_SetSpeed(-120,0,Yaw_Angle,180);
	} while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_LE, 160, 3));

	DelayTask_Add(1, 600, (void (*)(void)) change_flag, "%d%d", &flag, 0);
	while(flag)
	{
		vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
		Set_KeepDistance_X(0,0,Yaw_Angle,180,120);
	}
	flag=1;
	// /*====================回正====================*/
	 //Chassis_GuiWei(180);
	/*====================定位前准备1:小车移动到离开阶梯的位置====================*/
	do
	{
		/* code */
		vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
		Set_KeepDistance_X(0,-50,Yaw_Angle,180,120);

	} while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_GT, 150, 3));
	delay_ms(100);
	/*====================定位:小车回到阶梯的位置====================*/
	DelayTask_Add(1, 400, (void (*)(void)) change_flag, "%d%d", &flag, 0);
	while(flag)
	{
		Chassis_SetSpeed(0,50,Yaw_Angle,180);	
	}
	flag=1;
	Chassis_Stop();
	/*====================摄像头开始控制小车向右走====================*/
	DelayTask_Add(1, 2500, (void (*)(void)) change_flag, "%d%d", &flag, 0);
	while(flag)
	{
		Chassis_SetSpeed(0,70,Yaw_Angle,180);	
	}
	flag=1;
	Chassis_Stop();	
}


void Go_To_Small_Turntable(void)
{
	IM948_Reset();
	//前往小转盘
	/*====================先退下，给转弯一点空间====================*/
	Chassis_FixSpeed(200,0,0,275);					// 向右平移 275*2ms=550ms（速度翻倍，时间减半，距离不变）
	Chassis_Stop();									// 停车
	/*====================左转====================*/
	Chassis_TurnLeft(90);								// 左转90°
	Chassis_Stop();									// 停车
	delay_ms(10);									// 等陀螺仪/电机稳定
	/*====================原地旋转检测小圆盘====================*/
	do
	{
		Chassis_InverseMotionControl(0,0,250);		// 原地左转(w=250,逆时针)
		vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);	// 激光测距
	} while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_LT, 350, 5));	// 连续5次距离<350mm才停
	Chassis_Stop();					// 检测到小圆盘,停车
/*====================细调与小圆盘的距离====================*/
	while(1)
	{
		if(vl53l0x_data.RangeMilliMeter < 160)
		{
			break;
		}

		vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
		Chassis_InverseMotionControl(-50,0,0);
	}
	Chassis_Stop();	
	/*====================开始绕圈(绕一整圈后退出)====================*/
	Avoid_PID.Need_Value = 160;
	PID_PositionClean(&Avoid_PID);      // 清掉PID残留
	
	int zero_cnt = 0;                   // 经过0°的次数
	bool in_zero = false;               // 现在是否在0°附近(防抖动重复计数)

	while(zero_cnt < 1)                 // 第2次回到0°就退出
	{
		vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
		PID_PositionCalc(&Avoid_PID, vl53l0x_data.RangeMilliMeter);
		int vx = Avoid_PID.OUT*2;
		int vw = 250;
		int vy = vw * 0.335f;
		Chassis_InverseMotionControl(vx,vy,vw);

		/* ==== 判断是否进入90°附近 ==== */
		if(Yaw_Angle > 85 && Yaw_Angle < 92)
		{
			if(!in_zero) zero_cnt++;    // 每"进入"0°一次,计一次数
			in_zero = true;
		}
		else
		{
			in_zero = false;
		}


	}
	
	Chassis_Stop();
	delay_ms(500);
}


void Go_To_Warehouse(void)
{
	Chassis_AnglePID.Need_Value = Yaw_Angle;
	/*====================检测到仓库====================*/
	do
	{
		/* code */
		vl53l0x_start_single_test(&vl53l0x_dev1,&vl53l0x_data);
		Chassis_SetSpeed(0,-150,Yaw_Angle,90);
	} while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_LE, 300, 3));

	DelayTask_Add(1, 500, (void (*)(void)) change_flag, "%d%d", &flag, 0);
	while(flag)
	{
		vl53l0x_start_single_test(&vl53l0x_dev1,&vl53l0x_data);
		Set_KeepDistance_Y(0,0,Yaw_Angle,90,150);
	}
	flag=1;
	/*====================定位前准备1:小车移动到离开阶梯的位置====================*/
	delay_ms(10);
	do
	{
		/* code */
		vl53l0x_start_single_test(&vl53l0x_dev1,&vl53l0x_data);
		Chassis_SetSpeed(80,0,Yaw_Angle,90);		
	} while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_GT, 400, 3));
	delay_ms(5);
	Chassis_Stop();	
	delay_ms(5);
	/*====================定位:小车移动到阶梯第一个列====================*/
	DelayTask_Add(1, 1000, (void (*)(void)) change_flag, "%d%d", &flag, 0);
	while(flag)
	{
		Chassis_SetSpeed(-50,0,Yaw_Angle,90);		
		vl53l0x_start_single_test(&vl53l0x_dev1,&vl53l0x_data);	// 激光测距 PD0
	}
	flag=1;	
	delay_ms(5);
	Chassis_Stop();	
	delay_ms(5);	
	/*====================定位:小车行驶到缝隙处====================*/	
	/*====================定位:小车行驶到第一个缝隙处====================*/	
	do
	{
		/* code */
		vl53l0x_start_single_test(&vl53l0x_dev3,&vl53l0x_data);
		Chassis_SetSpeed(-50,0,Yaw_Angle,90);		
	} while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_LT, 280, 3));
	Chassis_Stop();
	delay_ms(1000);


	/*====================定位:小车行驶到第二个缝隙处====================*/	
	Chassis_SetSpeed(-80,0,Yaw_Angle,90);	
	delay_ms(200);	
	do
	{
		/* code */
		vl53l0x_start_single_test(&vl53l0x_dev3,&vl53l0x_data);
		Chassis_SetSpeed(-50,0,Yaw_Angle,90);		
	} while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_LT, 280, 3));
	Chassis_Stop();
	delay_ms(1000);	

	/*====================定位:小车行驶到第三个缝隙处====================*/	
	Chassis_SetSpeed(-80,0,Yaw_Angle,90);	
	delay_ms(200);	
	do
	{
		/* code */
		vl53l0x_start_single_test(&vl53l0x_dev3,&vl53l0x_data);
		Chassis_SetSpeed(-50,0,Yaw_Angle,90);		
	} while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_LT, 280, 3));
	Chassis_Stop();
	delay_ms(1000);	

	/*====================定位:小车行驶到第四个缝隙处====================*/	
	Chassis_SetSpeed(-80,0,Yaw_Angle,90);	
	delay_ms(200);	
	do
	{
		/* code */
		vl53l0x_start_single_test(&vl53l0x_dev3,&vl53l0x_data);
		Chassis_SetSpeed(-50,0,Yaw_Angle,90);		
	} while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_LT, 280, 3));
	Chassis_Stop();
	delay_ms(1000);	


	Chassis_GuiWei(90);
	Chassis_Stop();
	// DelayTask_Add(1, 800, (void (*)(void)) change_flag, "%d%d", &flag, 0);
	// while(flag)
	// {
	// 	Set_KeepDistance_Y(-50,0,Yaw_Angle,90,200);

	// 	vl53l0x_start_single_test(&vl53l0x_dev1,&vl53l0x_data);	// 激光测距 PD0
	// 	delay_ms(50);


	// }
	// flag=1;
	// Chassis_Stop();
	// // Chassis_GuiWei(90);
	
	// DelayTask_Add(1, 1000, (void (*)(void)) change_flag, "%d%d", &flag, 0);
	// while(flag)
	// {
	// 	Set_KeepDistance_Y(-50,0,Yaw_Angle,90,200);


	// 	vl53l0x_start_single_test(&vl53l0x_dev1,&vl53l0x_data);	// 激光测距 PD0
	// 	printf("dev2:%5dmm\r\n", vl53l0x_data.RangeMilliMeter);		// 打印距离(调试用)	
	// 	delay_ms(50);


	// }
	// flag=1;	
	// Chassis_Stop();
	// delay_ms(1000);
	// DelayTask_Add(1, 1000, (void (*)(void)) change_flag, "%d%d", &flag, 0);
	// while(flag)
	// {
	// 	Set_KeepDistance_Y(-50,0,Yaw_Angle,90,200);


	// 	vl53l0x_start_single_test(&vl53l0x_dev1,&vl53l0x_data);	// 激光测距 PD0
	// 	printf("dev2:%5dmm\r\n", vl53l0x_data.RangeMilliMeter);		// 打印距离(调试用)	
	// 	delay_ms(50);


	// }
	// flag=1;		
	// Chassis_Stop();
	// delay_ms(1000);
	// DelayTask_Add(1, 1000, (void (*)(void)) change_flag, "%d%d", &flag, 0);
	// while(flag)
	// {
	// 	Set_KeepDistance_Y(-50,0,Yaw_Angle,90,200);


	// 	vl53l0x_start_single_test(&vl53l0x_dev1,&vl53l0x_data);	// 激光测距 PD0
	// 	printf("dev2:%5dmm\r\n", vl53l0x_data.RangeMilliMeter);		// 打印距离(调试用)	
	// 	delay_ms(50);


	// }	
	// Chassis_Stop();
}	
void Go_To_Home(void)
{
	Chassis_MovePath(Chassis_Path_StartToHome);
	while (1)
	{
		Chassis_AnglePID.Need_Value = Yaw_Angle;
		Chassis_GuiWei(90);
		while(1)
		{
			
			u8 f = GRAY_front, r = GRAY_Right, b = GRAY_behind, l = GRAY_Left; // 0=红 1=黑

			if (f == 1) {Chassis_FixSpeed(0,-50,90,100); }   
			if (r == 1) {Chassis_FixSpeed(-50,0,90,100); }  
			if (b == 1) {Chassis_FixSpeed(0,50,90,100); }  
			if (l == 1) {Chassis_FixSpeed(50,0,90,100); }   
			Chassis_Stop();
			delay_ms(50);  // 每次移动后留稳定时间
			if (f == 0 && r == 0 && b == 0 && l == 0) { break; } // 全部为红色，退出循环
		}
		while(Range_ConsecutiveMatch_AutoCnt(GRAY_Right, CMP_EQ, 1, 20))
		{
			delay_ms(10);
			printf("GRAY_CH1:%d\n\r",GRAY_Right);	
			Chassis_SetSpeed(50,0,Yaw_Angle,90);
		}
		Chassis_Stop();	
		Chassis_FixSpeed(-50,0,90,150);
		Chassis_Stop();	
		while (1);
		Chassis_AnglePID.Need_Value = Yaw_Angle;
	}
}

