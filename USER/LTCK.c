#include "headfile.h"

bool Start_init = false;
Color Selected_Color = None;
volatile uint8_t flag = 1;
int overtime_flag=1;

volatile uint8_t turntable_task = 1;
volatile uint8_t turnplate_flag = 1;
volatile uint8_t ball_num = 0;
volatile uint8_t turntabel_end = 0;
uint8_t Disc_flag = 2;

volatile uint8_t Stairs_Task = 1;

volatile uint8_t delay_task=0;

void LTCK_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 设置系统中断优先级分组2
    /*
        优先级高低：外设 抢占优先级 子优先级

        陀螺仪                0   0
        电机（TIM5）          1   0
        延时函数（TIM7）      1   2
    */
    Gray_Init();                      // 灰度传感器
    LED_Init();                       // LED
    Infrared_Init();                  // 红外开关
    PwmServo_Init(20000 - 1, 84 - 1); // 初始化舵机 20ms 周期的 PWM 波（50Hz） --PA6
    KEY_Init();                       // 按键
    delay_init(168);                  // 延时函数
    usart6_init(230400);              // K230通信(原UART5, PC12/PD2 -> PG14/PG9)
    uart5_init(115200);               // printf 调试串口 (PC12=TX / PD2=RX)
    usart1_init(230400);              // 摄像头通信
    usart3_init(9600);                // 舵机控制板
    uart4_init(9600);                 // 读卡器
    usart2_init(115200);              // 陀螺仪
    IM948_Init();                     // 陀螺仪初始化
    delay_ms(2000);                   // 等待陀螺仪稳定
    can_init();                       // CAN总线初始化
    TIM5_Int_Init(42 - 1, 1500 - 1);  // 1.5ms 进入一次中断 底盘控制专用
    TIM6_Int_Init(100 - 1, 8400 - 1); // 10ms 进入一次中断
    TIM7_Int_Init(100 - 1, 8400 - 1); // 10ms进入一次中断，延时执行任务 （Delaytask.c）

    MYDMA_Config(DMA1_Stream3, DMA_Channel_4, (u32)&USART3->DR, (u32)LobotTxBuf, 7, 0);                // DMA1,STEAM3,CH4,外设为串口1,存储器为LobotTxBuf,长度为:SEND_BUF_SIZE.（只是用于发送执行动作组指令）
    MYDMA_Config(DMA1_Stream2, DMA_Channel_4, (u32)&UART4->DR, (u32)cx522_rxbuf, sizeof(cx522_rxbuf), 1);               // DMA1,STEAM2,CH4,外设为串口4,存储器为cx522_rxbuf,长度为sizeof(cx522_rxbuf)（DMA接收读卡器数据）
    MYDMA_Config(DMA1_Stream4, DMA_Channel_0, (u32)&SPI2->DR, (u32)pixelBuffer, Pixel_S1_NUM * 24, 0); // DMA1,STEAM4,CH0,外设为SPI2,存储器为pixelBuffer,长度为:Pixel_S1_NUM * 24（DMA发送WS2812像素数据）

    cx522_Init();             // 读卡器初始化
    VL53L0X_All_Init();       // 激光测距初始化
    WS2812b_Configuration();  // WS2812B像素灯初始化
    rgb_SetColor(RGB_1, RED);

    StepMotor_Init();   // 步进电机初始化
    Chassis_PathInit(); // 底盘路径初始化
    Chassis_PID_Init(); // 底盘PID初始化
    delay_ms(50);
    rgb_SetColor(RGB_1, YELLOW);
    DelayTask_Add(100000, 10, (void (*)(void))turnplate_detect_test, 0);
}

void Choose_Color(void)
{
    while (!Start_init)
    {   
        if (Key1_value)
        {
            Selected_Color++;
            if (Selected_Color > 3)
            {
                Selected_Color = None;
            }

            switch (Selected_Color)
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

        if (Key3_value) // 开始比赛，指示灯变绿
        {
            if (Selected_Color == Red || Selected_Color == Blue)
            {
                Start_init = true;
                rgb_SetColor(RGB_4, GREEN);
            }
            Key3_value = 0;
        }
    }
}

void Set_KeepDistance_X(float Vx, float Vy, float angle, float target_angle, float target_dis)
{

    Vx = 0.7 *(target_dis - vl53l0x_data.RangeMilliMeter);


    if (Vx > 300)
        Vx = 0;
    if (Vx < -300)
        Vx = 0;

    Chassis_SetSpeed(Vx, Vy, angle, target_angle);
}

void Set_KeepDistance_Y(float Vx, float Vy, float angle, float target_angle, float target_dis)
{
    if(vl53l0x_data.RangeMilliMeter>600)
    {
        Vy=0;
    }
    else
    {
        Vy = 1 * (target_dis - vl53l0x_data.RangeMilliMeter);
    }
    if (Vy > 200)
        Vy = 0;
    if (Vy < -200)
        Vy = 0;
    if (Vy<5&&Vy>-5)
    {
        Vy = 0;
    }
 
    Chassis_SetSpeed(Vx, Vy, angle, target_angle);
}

void change_flag(u8 *target_flag, u8 flag)
{
    *target_flag = flag;
}

void go_next_gap(void)
{
    Chassis_FixSpeed(-50,0,90,300);
    do
    {
        vl53l0x_start_single_test(&vl53l0x_dev3, &vl53l0x_data);
        Chassis_SetSpeed(-50, 0, Yaw_Angle, 90);
    } while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_LT, 400, 3));
    Chassis_Stop();
    delay_ms(1000);
}

int find_hole(uint16_t target_ic)
{
    for (int i = 0; i < 10; i++)
    {
        if (HoleArr[i].ic == target_ic)
        {
            return i;
        }
    }
    return -1;
}

void arm_finish_waiting(void)
{
    arm_overtime=0;  
    while(isActionGroupCompleted==0) //等待机械臂动作做完
    {
        delay_ms(1);
        arm_overtime++;
        if(arm_overtime>20000)
        {
            arm_overtime=0;
            break;
        }
    }       
}


void Go_To_Turntable(void)
{
    // uint8_t Disc_flag = 2;
    // // 前往大转盘
    // Chassis_MovePath(Chassis_Path_StartToTurntable);
    // 定位
    while (Range_ConsecutiveMatch_AutoCnt(GRAY_CH2, CMP_EQ, 0, 3))
    {
        Chassis_SetSpeed(-30, 0, Yaw_Angle, 0);
        delay_ms(50);
    }
    Chassis_Stop();
    delay_ms(5000);

    while (Range_ConsecutiveMatch_AutoCnt((GRAY_CH2 != 0 && GRAY_CH3!= 0), CMP_EQ, 1, 3))
    {
        Chassis_SetSpeed(0, -70, Yaw_Angle, 0);
        delay_ms(20);
    }


    Chassis_MoveOnce(0, 50, 0, 600, 100);
    Chassis_GuiWei(0,500);
    Chassis_Stop();



    while(turntabel_end==0)
    {
    	switch (turntable_task)
    	{
            case 0:
                Chassis_Stop();
                 break;
            case 1:
                runActionGroup(1,1,false);
                openmv_send("{1}");
                openmv_send("{S}");
                arm_finish_waiting();
                turntable_task=2;
                break;
            case 2:
                if(turnplate_flag==1)
				{
                    if (Ball_falling() == 0)
                    {

                        // HoleArr[Hole_Idx+3].ball = 0;
                        Hole_Idx++;
                        ball_num++;

                        if(5 == ball_num)
                        {
                            for(int i=0;i<10;i++)
                            {
                                printf("id:%d,ball:%d,ic:%#x\r\n",i,HoleArr[i].ball,HoleArr[i].ic);
                            }

                            printf("ending1");
                            return;
                        }
                        printf("%d",ball_num);
                        Turnplate_Move(Hole_Idx);
                        delay_ms(wait_time);
                        turnplate_flag=0;
                        DelayTask_Add(1,500,(void (*)(void)) change_flag,"%d%d",&turnplate_flag,1);
                    }
                }
                
                if(1 == openmv_rx_cpl)
                {
                    openmv_rx_command = 0;
                    openmv_rx_cpl=0;
                    sscanf(openmv_rxbuf,"{%c}",&openmv_rx_command);
        //            printf("openmv_rx_comm:%c",openmv_rx_command);

                    if(target_color == openmv_rx_command)
                    {
                        runActionGroup(2,1,false);
                        arm_finish_waiting();
                        openmv_rx_cpl = 0;
                    }


                }
                break;
    	}
        
    }


}



void Go_To_Stairs(void)
{

    // 前往阶梯
    while (Yaw_Angle > -60)
    {
        Chassis_InverseMotionControl(240, 0, -300);
    }
    while (Yaw_Angle > -90)
    {
        Chassis_InverseMotionControl(160, 0, -200);
    }
    Chassis_TurnRight();
    
    do
    {
        /* code */
        vl53l0x_start_single_test(&vl53l0x_dev2, &vl53l0x_data);
        Chassis_SetSpeed(-40, 120, Yaw_Angle, 180);
    } while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_LT, 400, 3));

    // /*====================检测到阶梯====================*/
    do
    {
        /* code */
        vl53l0x_start_single_test(&vl53l0x_dev2, &vl53l0x_data);
        Chassis_SetSpeed(-80, 0, Yaw_Angle, 180);
    } while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_LE, 130, 3));

    DelayTask_Add(1, 600, (void (*)(void))change_flag, "%d%d", &flag, 0);
    while (flag)
    {
        vl53l0x_start_single_test(&vl53l0x_dev2, &vl53l0x_data);
        Set_KeepDistance_X(0, 0, Yaw_Angle, 180, 90);
    }
    Chassis_Stop();
    flag = 1;
    // /*====================回正====================*/
    // Chassis_GuiWei(180,50);
    /*====================定位前准备1:小车移动到离开阶梯的位置====================*/
    do
    {
        /* code */
        vl53l0x_start_single_test(&vl53l0x_dev2, &vl53l0x_data);
        Chassis_SetSpeed(0, -40, Yaw_Angle, 180);
    } while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_GT, 150, 3));
    Chassis_Stop();
    /*====================定位:小车回到阶梯的位置====================*/
    DelayTask_Add(1, 400, (void (*)(void))change_flag, "%d%d", &flag, 0);
    while (flag)
    {
        Chassis_SetSpeed(0, 50, Yaw_Angle, 180);
    }
    flag = 1;
    Chassis_Stop();
    /*====================摄像头开始控制小车向右走====================*/
	u8 task = 1;
	u8 identified=0;
	int16_t Stairs_Err=0;
	float  Stairs_Chassis_Speed=0;	
    uint8_t soft_accel=0;	
    uint8_t stairs_ending=0;	
	while(stairs_ending==0)
	{
		switch (task)
		{
			case  0:
				Chassis_Stop();
				break;
			case  1:
				openmv_rx_cpl=0;  	
				openmv_rx_stair=0;
				openmv_rx_stair_dis=0;

			    runActionGroup(3, 1, false);   
                openmv_send("{2}");
                arm_finish_waiting();

				task=2;
				break;
			
			case  2:
                vl53l0x_start_single_test(&vl53l0x_dev2, &vl53l0x_data);
				if(vl53l0x_data.RangeMilliMeter > 150)               // 冲过头了/楼梯不在
				{
					delay_ms(20);
					Chassis_Stop();
					delay_ms(20);
					stairs_ending=1;
				}								
				if (identified==1)
				{		
						
					if (openmv_rx_cpl == 1)
					{
						sscanf(openmv_rxbuf, "{%d,%d}",&openmv_rx_stair,&openmv_rx_stair_dis);	
						Stairs_Err=openmv_rx_stair_dis-640;
                        printf("%d",Stairs_Err);
						Stairs_Chassis_Speed=Stairs_Err*0.10;
						if (Stairs_Chassis_Speed>200)Stairs_Chassis_Speed=200;
						if (Stairs_Chassis_Speed<-200)Stairs_Chassis_Speed=-200;
						if (Stairs_Chassis_Speed<0)Stairs_Chassis_Speed=0;
                        Set_KeepDistance_X(0, Stairs_Chassis_Speed+20, Yaw_Angle, 180, 90);		
						if (Stairs_Err>-70&&Stairs_Err<70)
						{
							Chassis_SINAccel(0, 27, 0, 0, Yaw_Angle, 250);
							DelayTask_Add(1,500,(void (*)(void)) change_flag,"%d%d",&task,3);
							identified=0;
							task=0;
						}
						openmv_rx_cpl=0;
					}
				}
				else
				{		
                    vl53l0x_start_single_test(&vl53l0x_dev2, &vl53l0x_data);							
					if (openmv_rx_cpl == 1)
					{
						openmv_rx_stair=0;					
						sscanf(openmv_rxbuf, "{%d,%d}",&openmv_rx_stair,&openmv_rx_stair_dis);	
                        // printf("%d",openmv_rx_stair);                  
						if(openmv_rx_stair!=0)	
						{
							identified	=1;
						}
						openmv_rx_cpl=0;						
					}
                    if(soft_accel==0)//缓加速,为了不晃,下一轮开始时重新缓加速
                    {
                        soft_accel=1;
                        Chassis_SINAccel(0, 0, 0, 35, Yaw_Angle, 400);
                    }
                    else
                    {
                        Set_KeepDistance_X(0, 35, Yaw_Angle, 180, 90);
                    }									
				}break;	
			case 3:
				Chassis_Stop();
				if (openmv_rx_stair==3)
				{
					runActionGroup(4, 1, false);
                    arm_finish_waiting();
				}
				else if (openmv_rx_stair==2)
				{
					runActionGroup(5, 1, false);
                    arm_finish_waiting();
				}
				else if (openmv_rx_stair==1)
				{
					runActionGroup(6, 1, false);
                    arm_finish_waiting();
				}
				task=4;
				break;
			case 4:
				if(turnplate_flag==1)
				{
                    rgb_SetColor(RGB_3, RED);
					if (Ball_falling() == 0)
					{
                        vl53l0x_start_single_test(&vl53l0x_dev2, &vl53l0x_data);
						HoleArr[Hole_Idx+3].ball = 0;
						Hole_Idx++;	

						Turnplate_Move(Hole_Idx);
                        delay_ms(wait_time+200);
						DelayTask_Add(1,400,(void (*)(void)) change_flag,"%d%d",&task,2);	

						DelayTask_Add(1,500,(void (*)(void)) change_flag,"%d%d",&turnplate_flag,1);
						turnplate_flag = 0;
                        soft_accel=0;
					}
				}
				break;	
		}
	}
}

void Go_To_Small_Turntable(void)
{
    IM948_Reset();
    Chassis_AnglePID.Need_Value = Yaw_Angle;
    // 前往小转盘
    /*====================先退下，给转弯一点空间====================*/
    Chassis_FixSpeed(90, -35, 0, 600); // 向右平移 275*2ms=550ms（速度翻倍，时间减半，距离不变）
    Chassis_Stop();                   // 停车
    /*====================左转====================*/
    Chassis_TurnLeft(135); // 左转135°
    Chassis_Stop();       // 停车
    delay_ms(20);         // 等陀螺仪/电机稳定

    /*====================原地旋转检测小圆盘,写死====================*/
	do
    {
        Chassis_InverseMotionControl(0, 0, 150);                 // 原地左转(w=250,逆时针)
        vl53l0x_start_single_test(&vl53l0x_dev2, &vl53l0x_data); // 激光测距
    } while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_LT, 350, 3)); // 连续次距离<350mm才停
    do
    {
        Chassis_InverseMotionControl(0, 0, 150);                 // 继续左转(w=250,逆时针)
        vl53l0x_start_single_test(&vl53l0x_dev2, &vl53l0x_data); // 激光测距
    } while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_GE, 500, 3)); // 连续次距离<350mm才停
    Chassis_Stop(); // 检测到小圆盘,停车


    delay_ms(200);   

    DelayTask_Add(1, 500, (void (*)(void))change_flag, "%d%d", &flag, 0);
    while (flag)
    {
        Chassis_InverseMotionControl(0, 0, -150);  
    }
		Chassis_Stop();
    flag = 1;  
    delay_ms(200);   
 
    /*====================细调与小圆盘的距离====================*/
                
    while (vl53l0x_data.RangeMilliMeter < 180)
    {
        vl53l0x_start_single_test(&vl53l0x_dev2, &vl53l0x_data);
        Chassis_InverseMotionControl(-25, 0, 0);
    }
    Chassis_Stop();
    printf("stop1");
    delay_ms(200);

    DelayTask_Add(1, 2000, (void (*)(void))change_flag, "%d%d", &flag, 0);
    Chassis_AnglePID.Need_Value = Yaw_Angle;
    while (flag)
    {
        vl53l0x_start_single_test(&vl53l0x_dev2, &vl53l0x_data);
        Set_KeepDistance_X(0, 0, Yaw_Angle, Yaw_Angle, 150);
    }
    Chassis_Stop();
    flag = 1;
    
    /*====================开始绕圈====================*/
    vl53l0x_start_single_test(&vl53l0x_dev2, &vl53l0x_data);
    printf("%d",vl53l0x_data.RangeMilliMeter);
  	uint8_t stake_flag= 1;
    uint8_t stake_num= 0;
    uint8_t delay=0;

    runActionGroup(7, 1, false);
    arm_finish_waiting();
    
    openmv_send("{S}");  		
    openmv_send("{3}");

    // openmv_rx_cpl = 0;
    // openmv_rxbuf[0] = 0;
    
    Avoid_PID.Need_Value = 150;
    PID_PositionClean(&Avoid_PID);      // 清掉PID残留
    while(stake_flag!=10)                 // 推到两个球,然后碰到90°就退出
    {
        vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
        // printf("vl53l0x_dev2: %d\r\n", vl53l0x_data.RangeMilliMeter);

        PID_PositionCalc(&Avoid_PID, vl53l0x_data.RangeMilliMeter);
        int vx = Avoid_PID.OUT*2;
        int vw = -250;
        int vy = vw * 0.350;//200+Need_Value
        Chassis_InverseMotionControl(vx,vy,vw);

        switch (stake_flag)
        {
            case 1:
                if(openmv_rx_cpl==1)
                {
                    openmv_rx_command = 0;
                    sscanf(openmv_rxbuf,"{%c}",&openmv_rx_command);
                    // printf("openmv_rx_command:%c",openmv_rx_command);
                    if(openmv_rx_command==target_color)
                    // if(openmv_rx_command!=0)
                    {
                        if(delay==0)
                        {
                            if(stake_num==0)
                            {
                                openmv_rx_command = 0;
                                stake_num=1;
                            }
                            else
                            {
                                openmv_rx_command = 0;
                                runActionGroup(8, 1, false);                   
                            }	
                            delay=1;
                            DelayTask_Add(1,1500,(void (*)(void)) change_flag,"%d%d",&delay,0);//等待机械臂动作
                        }
                    }
                    openmv_rx_cpl = 0;
                }				
                if(turnplate_flag==1)
                {

                    if (Ball_falling() == 0)
                    {
                        rgb_SetColor(RGB_3, RED);
                        HoleArr[Hole_Idx].ball = 0;
                        Hole_Idx++;
                        ball_num++;
                        Turnplate_Move(Hole_Idx);
                        if(stake_num==1)
                        {
                            stake_num=2;
                        }
                        else if(stake_num==2)
                        {
                            stake_flag=3;
                        }

                        DelayTask_Add(1,1000,(void (*)(void)) change_flag,"%d%d",&turnplate_flag,1);
                        turnplate_flag = 0;
                    }
                    
                }					
                break;
            case 3:
            
                /* ==== 判断是否进入90°附近 ==== */
                if(Yaw_Angle > 85 && Yaw_Angle < 92)
                {
                    stake_flag=10;
                }
                break;					
        }
    }
    
    Chassis_Stop();
}



void Go_To_Warehouse(void)
{

		HoleArr[3].ball=0;
		// HoleArr[1].ball=0;
		// HoleArr[2].ball=0;
		HoleArr[4].ball=0;
		HoleArr[5].ball=0;
		HoleArr[6].ball=0;
		// HoleArr[7].ball=0;
		// HoleArr[8].ball=0;
		// HoleArr[9].ball=0;				
		HoleArr[3].ic=0x11;
		// HoleArr[1].ic=0x21;
		// HoleArr[2].ic=0x31;		
		HoleArr[4].ic=0x12;
		HoleArr[5].ic=0x22;
		HoleArr[6].ic=0x32;		
		// HoleArr[7].ic=0x13;
		// HoleArr[8].ic=0x23;
		// HoleArr[9].ic=0x33;	



   /*====================test:先不管仓库列的顺序,只处理第一列有没有球====================*/

		uint8_t warehouse_row = 0;
		uint8_t warehouse_col = 0;
        uint8_t warehouse_task = 1;
        uint8_t warehouse_task_end = 0;

        uint8_t warehouse_identify_ok = 0; 

        uint8_t block_number= 0;

		/*====================openmv检测数字====================*/
        delay_ms(10);
		Chassis_AnglePID.Need_Value = Yaw_Angle;

		// k230_send_command(6);

		// Chassis_TurnLeft(45);

		// Chassis_AnglePID.Need_Value = Yaw_Angle; //可以试一下放在Chassis_TurnLeft()

        // layer[0]=0;
        // layer[1]=0;
        // layer[2]=0;

		// DelayTask_Add(1, 1000, (void (*)(void))change_flag, "%d%d", &overtime_flag, 0);
		// while (overtime_flag)
		// {

        //     // k230_process();
        //     // if(k230_rx_ok == 1)
        //     // {
        //     //     rgb_SetColor(RGB_2, Blue);
        //     //     while(1);
        //     // }

        //     if (openmv_rx_cpl == 1)
        //     {
        //         /*====================成功获得六个数====================*/
        //         if (sscanf(openmv_rxbuf, "{%d,%d,%d,%d,%d,%d}", &layer[0],&openmv_warehouse_block_3,
        //                     &layer[1],&openmv_warehouse_block_2,
        //                     &layer[2],&openmv_warehouse_block_1 ) == 6)
        //             {
        //                 overtime_flag=0;
        //                 warehouse_identify_ok=1;
        //             }
                
        //         openmv_rx_cpl = 0;
        //     }		    
		// }

        overtime_flag=1;
        
        if(warehouse_identify_ok!=1)
        {
            layer[0] = layer[1] = layer[2] = 0;
            openmv_warehouse_block_1 = openmv_warehouse_block_2 = openmv_warehouse_block_3 = 0;            
        }      

		/*====================检测到仓库====================*/
        runActionGroup(14, 1, false);//抓取转盘的球放到仓库 

        do
        {
            /* code */
            vl53l0x_start_single_test(&vl53l0x_dev1, &vl53l0x_data);
            Chassis_SetSpeed(-20, -100, Yaw_Angle, 90);
        } while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_LE, 400, 3));

        do
        {
            /* code */
            vl53l0x_start_single_test(&vl53l0x_dev1, &vl53l0x_data);
            Chassis_SetSpeed(0, -40, Yaw_Angle, 90);
        } while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_LE, 250, 3));


        DelayTask_Add(1, 500, (void (*)(void))change_flag, "%d%d", &flag, 0);
        while (flag)
        {
            vl53l0x_start_single_test(&vl53l0x_dev1, &vl53l0x_data);
            Set_KeepDistance_Y(0, 0, Yaw_Angle, 90, 200);
        }
        flag = 1;
        /*====================定位前准备1:小车移动到离开阶梯的位置====================*/
        delay_ms(10);
        do
        {
            /* code */
            vl53l0x_start_single_test(&vl53l0x_dev1, &vl53l0x_data);
            Chassis_SetSpeed(30, 0, Yaw_Angle, 90);
        } while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_GT, 400, 3));
        delay_ms(5);
        Chassis_Stop();
        delay_ms(5);
        /*====================定位:小车移动到阶梯的位置====================*/
        Chassis_FixSpeed(-30,0,90,300);
     /*====================定位:小车行驶到第一个缝隙处====================*/
		go_next_gap();
        DelayTask_Add(1, 500, (void (*)(void))change_flag, "%d%d", &flag, 0);
        Chassis_AnglePID.Need_Value = Yaw_Angle;
        printf("Yaw:%d\r\n", (int)Yaw_Angle);
        while (flag)
        {
            Chassis_GuiWei(90,1500);

        }
        Chassis_Stop();
        flag = 1;       
             
            /*================处理第一列第三行=============*/
   		warehouse_row = 3;
		warehouse_col = 1;
        while(warehouse_task_end==0)
        {
            switch(warehouse_task)
            {
                case 1:
                    while (warehouse_row>0)
                    {
                        if(warehouse_col==layer[3 - warehouse_row]) 
                        {
                            if      (warehouse_row == 3) block_number = openmv_warehouse_block_3;
                            else if (warehouse_row == 2) block_number = openmv_warehouse_block_2;
                            else if (warehouse_row == 1) block_number = openmv_warehouse_block_1;
                            if (block_number >= 1 && block_number <= 3)  // 数字在1~3才清,防止无效动作组卡20秒
                            {
                                //抓取仓库的积木,放到车上固定的位置,3,2,1,有53,52,51,43,42,41,33,32,31
                                runActionGroup((((warehouse_row+2)<<4)|block_number), 1, false);
                                arm_finish_waiting();                                                     
                            }
                            
                            
                            hole =find_hole( ( (warehouse_row << 4) | warehouse_col));
                            if(hole!=-1)
                            {
                                Turnplate_Move(hole+4);
                                delay_ms(wait_time+200);
                                runActionGroup(10+warehouse_row, 1, false);//抓取转盘的球放到仓库   13,12,11
                                arm_finish_waiting();   
                                // delay_ms(1500);                  
                            }           
                            warehouse_row--;                     
                        }
                        else
                        {
                            hole = find_hole(((warehouse_row << 4) | warehouse_col));
                            if(hole != -1)
                            {
                                Turnplate_Move(hole+4);
                                delay_ms(wait_time+200);              // 等转盘真正转到位
                                runActionGroup(10+warehouse_row, 1, false);
                                arm_finish_waiting();                       // 等机械臂做完(自带20s兜底)
                            }
                            warehouse_row--;
                        }
                    }
                    warehouse_task=2;
                    break;
                case 2:
                    if (warehouse_col < 3)      // 还有下一列
                    {
                        printf("stop6");   
                        go_next_gap();          // 移到下一个缝隙
                        warehouse_col++;
                        warehouse_row = 3;
                        warehouse_task = 1;
                    }
                    else                        // 三列都处理完了
                    {
                        warehouse_task = 3;
                    }
                    break;
                case 3:
                    // // 从车上固定位置(数字3/2/1对应位)放回仓库;识别失败时可能拿空,但不影响完赛
                    // runActionGroup(63, 1, false); arm_finish_waiting();
                    // runActionGroup(62, 1, false); arm_finish_waiting();
                    // runActionGroup(61, 1, false); arm_finish_waiting();

                    go_next_gap();         
                    warehouse_task_end=1;                 
                    break;
                                                                      
            }
            
        }         


    
}
void Go_To_Home(void)
{
    Chassis_AnglePID.Need_Value = Yaw_Angle;
    Chassis_MovePath(Chassis_Path_StartToHome);
    Chassis_AnglePID.Need_Value = Yaw_Angle;
    while (1)
    {
        Chassis_GuiWei(90,1000);
        while (1)
        {

            u8 f = GRAY_front, r = GRAY_Right, b = GRAY_behind, l = GRAY_Left; // 0=红 1=黑

            if (f == 1)
            {
                Chassis_FixSpeed(0, -50, 90, 100);
            }
            if (r == 1)
            {
                Chassis_FixSpeed(-50, 0, 90, 100);
            }
            if (b == 1)
            {
                Chassis_FixSpeed(0, 50, 90, 100);
            }
            if (l == 1)
            {
                Chassis_FixSpeed(50, 0, 90, 100);
            }
            Chassis_Stop();
            delay_ms(50); // 每次移动后留稳定时间
            if (f == 0 && r == 0 && b == 0 && l == 0)
            {
                break;
            } // 全部为红色，退出循环
        }
        while (Range_ConsecutiveMatch_AutoCnt(GRAY_Right, CMP_EQ, 1, 20))
        {
            delay_ms(10);
            // printf("GRAY_CH1:%d\n\r", GRAY_Right);
            Chassis_SetSpeed(50, 0, Yaw_Angle, 90);
        }
        Chassis_FixSpeed(-50, 0, 90, 200);
        Chassis_Stop();

        while (Range_ConsecutiveMatch_AutoCnt(GRAY_front, CMP_EQ, 1, 20))
        {
            delay_ms(10);
            // printf("GRAY_CH1:%d\n\r", GRAY_front);
            Chassis_SetSpeed(0, 50, Yaw_Angle, 90);
        }
        Chassis_FixSpeed(0, -50, 90, 200);
        Chassis_Stop();		

        while (1);
        Chassis_AnglePID.Need_Value = Yaw_Angle;
    }
}
