#include "headfile.h"

////////////////////////////////////参数初始化/////////////////////////////////////

//	APB1/APB1_TIM	42MHz
//	APB2/APB2_TIM	84MHz



//测试openmv发回数据控制底盘	
// static void Print_Stairs_Info_Task(void)
// {
// 			// printf("openmv_rx_stair_dis=%d, Stairs_Chassis_Speed=%d\r\n",
// 			//        openmv_rx_stair_dis, (int)Stairs_Chassis_Speed);
// 		printf("cengshu=%d, ",openmv_rx_stair);
// }


// int judge_hole(int16_t ic)
// {
// 	for (int i = 0; i < 10; i++)
// 	{
// 		if (ic == existed_block[0] || HoleArr[i].ic == existed_block[1] || HoleArr[i].ic == existed_block[2])
// 		{
// 			return 1;
// 		}
// 	}
// 	return 0;
// }


int main(void)
{
	/*====================初始化====================*/
	LTCK_Init();
	// runActionGroup(1,1,false);

	//k230test
	k230_send_command(6);
	// detect_allow=0;
	// DelayTask_Add(1000000, 200, (void (*)(void))Print_Stairs_Info_Task, NULL); 
	/*==f==================选择红蓝方====================*/
 	Choose_Color();	
	// Chassis_FixSpeed(0,40,180,300);
		// HoleArr[3].ball=0;
		// HoleArr[1].ball=0;
		// HoleArr[2].ball=0;
		// HoleArr[4].ball=0;
		// HoleArr[5].ball=0;
		// HoleArr[6].ball=0;
		// HoleArr[7].ball=0;
		// HoleArr[8].ball=0;
		// HoleArr[9].ball=0;				
		// HoleArr[3].ic=0x11;
		// HoleArr[1].ic=0x21;
		// HoleArr[2].ic=0x31;		
		// HoleArr[4].ic=0x12;
		// HoleArr[5].ic=0x22;
		// HoleArr[6].ic=0x32;		
		// HoleArr[7].ic=0x13;
		// HoleArr[8].ic=0x23;
		// HoleArr[9].ic=0x33;		
		// detect_allow=1;					
		

		// HoleArr[3].ball=0;
		// HoleArr[1].ball=0;
		// HoleArr[2].ball=0;
		// detect_allow=1;	
		// delay_ms(2000);
	// printf("target_color:%c\r\n", target_color);
 	/*====================前往大转盘====================*/
 	// Go_To_Turntable();
	/*====================前往楼梯====================*/
 	// Go_To_Stairs();
	/*====================前往小圆盘====================*/
  	// Go_To_Small_Turntable();
	/*====================前往仓库====================*/
  	// Go_To_Warehouse();
 	/*====================前往====================*/
 	// Go_To_Home();
	// runActionGroup(3, 1, false);  
	while (1)
	{
		//测试机械臂
		// if(isActionGroupCompleted==1)
		// {
		// 	rgb_SetColor(RGB_2, BLUE);
		// }
		// delay_ms(1000);
		// if(isActionGroupCompleted==0)
		// {
		// 	rgb_SetColor(RGB_2, RED);
		// }
		// delay_ms(1000);	
		
		

		// k230_process();	
        k230_process();
        if (k230_rx_ok == 1)
        {
            if (k230_d1 == 1 && k230_d2 == 2 && k230_d3 == 3)
            {
                rgb_SetColor(RGB_2, RED);
            }
            else
            {
                rgb_SetColor(RGB_2, BLUE);
            }

            k230_rx_ok = 0;
        }


		
		// 完成小转盘任务
		// uint8_t stake_flag= 1;
		// uint8_t stake_num= 0;
		// runActionGroup(0, 1, false);
		// openmv_send("{S}");  		
		// openmv_send("{3}");
		
		// Avoid_PID.Need_Value = 150;
		// PID_PositionClean(&Avoid_PID);      // 清掉PID残留
		// while(stake_flag!=10)                 // 推到两个球,然后碰到90°就退出
		// {
		// 	vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
		// 	// printf("vl53l0x_dev2: %d\r\n", vl53l0x_data.RangeMilliMeter);

		// 	PID_PositionCalc(&Avoid_PID, vl53l0x_data.RangeMilliMeter);
		// 	int vx = Avoid_PID.OUT*2;
		// 	int vw = -250;
		// 	int vy = vw * 0.350;
 		// 	Chassis_InverseMotionControl(vx,vy,vw);

		// 	switch (stake_flag)
		// 	{
		// 		case 1:
		// 			if(openmv_rx_cpl==1)
		// 			{
		// 				openmv_rx_cpl = 0;
		// 				openmv_rx_command = 0;
		// 				sscanf(openmv_rxbuf,"{%c}",&openmv_rx_command);
		// 				// printf("openmv_rx_command:%c",openmv_rx_command);
		// 				if(openmv_rx_command!=0)
		// 				{
		// 					DelayTask_Add(1, 500, (void (*)(void)) runActionGroup, "%d%d%d", 7, 1, false);					
		// 				}
		// 			}				
		// 			if(turnplate_flag==1)
		// 			{
		// 				if (Ball_falling() == 0)
		// 				{

		// 					HoleArr[Hole_Idx].ball = 0;
		// 					Hole_Idx++;
		// 					ball_num++;
		// 					Turnplate_Move(Hole_Idx);
		// 					if(stake_num==0)
		// 					{
		// 						stake_num=1;
		// 					}
		// 					else 
		// 					{
		// 						stake_flag=3;
		// 					}

		// 					DelayTask_Add(1,500,(void (*)(void)) change_flag,"%d%d",&turnplate_flag,1);
		// 					turnplate_flag = 0;
		// 				}
						
		// 			}					
		// 			break;
		// 		case 3:
		// 			/* ==== 判断是否进入90°附近 ==== */
		// 			if(Yaw_Angle > 85 && Yaw_Angle < 92)
		// 			{
		// 				stake_flag=10;
		// 			}
		// 			break;					
		// 	}
		// }
		
		// Chassis_Stop();
		// while(1);

		//阶梯联动测试
	// u8 task = 1;
	// u8 identified=0;
	// int16_t Stairs_Err=0;
	// float  Stairs_Chassis_Speed=0;			
	// while(task!=10)
	// {
	// 	switch (task)
	// 	{
	// 		case  0:
	// 			Chassis_Stop();
	// 			break;
	// 		case  1:
			
	// 			// int16_t Stairs_Err=0;
	// 			// float  Stairs_Chassis_Speed=0;

	// 			openmv_rx_cpl=0;  	
	// 			openmv_rx_stair=0;
	// 			openmv_rx_stair_dis=0;

	// 		    runActionGroup(3, 1, false);   
		
	// 			openmv_send("{R}");
	// 			delay_ms(5);
	// 			openmv_send("{2}");
	// 			delay_ms(5);
	// 			openmv_send("{S}");
	// 			delay_ms(5);


	// 			task=2;
	// 			break;
			
	// 		case  2:
	// 			vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
	// 			if(vl53l0x_data.RangeMilliMeter > 150)               // 冲过头了/楼梯不在
	// 			{
	// 				delay_ms(20);
	// 				Chassis_Stop();
	// 				delay_ms(20);
	// 				task=10;
	// 			}								
	// 			if (identified==1)
	// 			{		
						
	// 				if (openmv_rx_cpl == 1)
	// 				{
	// 					sscanf(openmv_rxbuf, "{%d,%d}",&openmv_rx_stair,&openmv_rx_stair_dis);	
	// 					Stairs_Err=openmv_rx_stair_dis-155;
	// 					Stairs_Chassis_Speed=Stairs_Err*0.5;
	// 					if (Stairs_Chassis_Speed>200)Stairs_Chassis_Speed=200;
	// 					if (Stairs_Chassis_Speed<-200)Stairs_Chassis_Speed=-200;
	// 					if (Stairs_Chassis_Speed<0)Stairs_Chassis_Speed=0;
	// 					Chassis_InverseMotionControl(0,Stairs_Chassis_Speed,0);		
	// 					if (Stairs_Err>-10&&Stairs_Err<10)
	// 					{
	// 						Chassis_Stop();
	// 						DelayTask_Add(1,1000,(void (*)(void)) change_flag,"%d%d",&task,3);
	// 						identified=0;
	// 						task=0;
	// 					}
	// 					openmv_rx_cpl=0;
	// 				}
	// 			}
	// 			else
	// 			{
	// 				// vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
	// 				// if(vl53l0x_data.RangeMilliMeter > 150)               // 冲过头了/楼梯不在
	// 				// {
	// 				// 	printf("stair_out");
	// 				// 	delay_ms(20);
	// 				// 	Chassis_Stop();
	// 				// 	delay_ms(20);
	// 				// 	while(1);
	// 				// }										
	// 				if (openmv_rx_cpl == 1)
	// 				{
	// 					printf("task_2");
	// 					openmv_rx_stair=0;					
	// 					sscanf(openmv_rxbuf, "{%d,%d}",&openmv_rx_stair,&openmv_rx_stair_dis);	
	// 					if(openmv_rx_stair!=0)	
	// 					{
	// 						identified	=1;
	// 					}
	// 					openmv_rx_cpl=0;						
	// 				}
	// 				printf("stair_test");				
	// 				Chassis_InverseMotionControl(0,30,0);						
	// 			}break;	
	// 		case 3:
	// 			Chassis_Stop();
	// 			if (openmv_rx_stair==3)
	// 			{
	// 				runActionGroup(4, 1, false);
	// 			}
	// 			else if (openmv_rx_stair==2)
	// 			{
	// 				runActionGroup(5, 1, false);
	// 			}
	// 			else if (openmv_rx_stair==1)
	// 			{
	// 				runActionGroup(6, 1, false);
	// 			}
	// 			task=4;
	// 			break;
	// 		case 4:
	// 			if(turnplate_flag==1)
	// 			{
	// 				if (Ball_falling() == 0)
	// 				{
	// 					HoleArr[Hole_Idx+3].ball = 0;
	// 					Hole_Idx++;	

	// 					Turnplate_Move(Hole_Idx);
	// 					Chassis_InverseMotionControl(0,30,0);	
	// 					DelayTask_Add(1,400,(void (*)(void)) change_flag,"%d%d",&task,2);	

	// 					DelayTask_Add(1,500,(void (*)(void)) change_flag,"%d%d",&turnplate_flag,1);
	// 					turnplate_flag = 0;
	// 				}
	// 			}	
	// 			break;	
	// 	}
	// }



		//测试激光测距
		// vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
		// printf("dev2:%5dmm\r\n", vl53l0x_data.RangeMilliMeter);
		// delay_ms(1000);

		// 测试openmv发回数据控制底盘
		// switch (task)
		// {
		// case  1:
		// 	openmv_rx_cpl=0;  	
		// 	openmv_rx_stair=0;
		// 	openmv_rx_stair_dis=0;

		// 	openmv_send("{R}");
		// 	openmv_send("{2}");
		// 	openmv_send("{S}");

		// 	DelayTask_Add(1000000, 200, (void (*)(void))Print_Stairs_Info_Task, NULL);
		// 	task=2;
		// 	break;
		
		// case  2:
		// 	if (openmv_rx_cpl == 1)
		// 	{
		// 		sscanf(openmv_rxbuf, "{%d,%d}",&openmv_rx_stair,&openmv_rx_stair_dis);	
		// 		Stairs_Err=155-openmv_rx_stair_dis;
		// 		Stairs_Chassis_Speed=Stairs_Err*0.8;
		// 		if (Stairs_Chassis_Speed>200)Stairs_Chassis_Speed=200;
		// 		if (Stairs_Chassis_Speed<-200)Stairs_Chassis_Speed=-200;
		// 		if (Stairs_Err>-5&&Stairs_Err<5)Stairs_Chassis_Speed=0;
		// 		Chassis_InverseMotionControl(0,-Stairs_Chassis_Speed,0);			
		// 	}
		// 	openmv_rx_cpl=0;
		// 	break;
		// }


	//	机械臂推球+转盘自转
		// switch (turntable_task)
		// {
		// case 1:
		// 	runActionGroup(1,1,false); 
		// 	DelayTask_Add(1,1000,(void (*)(void)) change_flag,"%d%d",&turntable_task,2);
		// 	turntable_task=0;
		// 	break;
		// case 2:
		// 	if(turnplate_flag==1)
		// 	{
		// 		if (Ball_falling() == 0)
		// 		{

		// 			HoleArr[Hole_Idx].ball = 0;
		// 			Hole_Idx++;
		// 			ball_num++;

		// 			if(6 == ball_num)
		// 			{
		// 				for(int i=0;i<10;i++)
		// 				{
		// 					printf("id:%d,ball:%d,ic:%#x\r\n",i,HoleArr[i].ball,HoleArr[i].ic);
		// 				}
		// 				turntabel_end =1;
		// 				while(1);
		// 			}

		// 			Turnplate_Move(Hole_Idx);

		// 			DelayTask_Add(1,500,(void (*)(void)) change_flag,"%d%d",&turnplate_flag,1);
		// 			turnplate_flag = 0;
		// 		}
				
		// 	}		
		// 	if(2 == Disc_flag)
		// 	{
		// 		DelayTask_Add(1, 1000, (void (*)(void)) runActionGroup, "%d%d%d", 2, 1, false);
		// 		DelayTask_Add(1,1200,(void (*)(void)) change_flag,"%d%d",&Disc_flag,3);
		// 		Disc_flag = 0;
		// 	}
		// 	else if(3 == Disc_flag)
		// 	{
		// 		DelayTask_Add(1,500,(void (*)(void)) change_flag,"%d%d",&Disc_flag,2);
		// 		Disc_flag = 0;
		// 	}
		// 	break;
		// }

		


	//转盘检测读ic卡,定时转圈	
		// if(turnplate_flag==1)
		// {
		// 	if (Ball_falling() == 0)
		// 	{
		// 		HoleArr[Hole_Idx+3].ball = 0;
		// 		Hole_Idx++;
		// 		ball_num++;
		// 		printf("value=%d\r\n" ,Hole_Idx);
		// 		if(6 == ball_num)
		// 		{
		// 			for(int i=0;i<10;i++)
		// 			{
		// 				printf("id:%d,ball:%d,ic:%#x\r\n",i,HoleArr[i].ball,HoleArr[i].ic);
		// 			}
		// 			turntabel_end =1;
		// 			detect_allow=1;
		// 			while(1);
		// 		}

		// 		Turnplate_Move(Hole_Idx);

		// 		DelayTask_Add(1,500,(void (*)(void)) change_flag,"%d%d",&turnplate_flag,1);
		// 		turnplate_flag = 0;
		// 	}
			
		// }			
	
		 /*====================测试灰度====================*/
		// printf("GRAY_CH1:%d\n\r", GRAY_CH1);
		// printf("GRAY_CH2:%d\n\r", GRAY_CH2);
		// printf("GRAY_CH3:%d\n\r", GRAY_CH3);
		// printf("GRAY_CH4:%d\n\r", GRAY_CH4);
		// printf("GRAY_CH5:%d\n\r", GRAY_CH5);
		// printf("GRAY_CH6:%d\n\r", GRAY_CH6);
		// printf("GRAY_CH7:%d\n\r", GRAY_CH7);
		// delay_ms(1000);

	}
}
