#include "headfile.h"

////////////////////////////////////参数初始化/////////////////////////////////////

//	APB1/APB1_TIM	42MHz
//	APB2/APB2_TIM	84MHz



//测试openmv发回数据控制底盘	
	u8 task=1;
	u8 identified=0;
	int16_t Stairs_Err=0;
	float  Stairs_Chassis_Speed=0;		
static void Print_Stairs_Info_Task(void)
{
			// printf("openmv_rx_stair_dis=%d, Stairs_Chassis_Speed=%d\r\n",
			//        openmv_rx_stair_dis, (int)Stairs_Chassis_Speed);
		printf("cengshu=%d, ",openmv_rx_stair);
}

int main(void)
{
	/*====================初始化====================*/
	LTCK_Init();

	detect_allow=1;
	turnplate_flag=1;

	runActionGroup(6, 1, false);
	DelayTask_Add(1000000, 200, (void (*)(void))Print_Stairs_Info_Task, NULL);
	/*==f==================选择红蓝方====================*/
 	Choose_Color();
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
	
	while (1)
	{
		// switch (task)
		// {
		// 	case  0:
		// 		Chassis_Stop();
		// 		break;
		// 	case  1:
		// 		// int16_t Stairs_Err=0;
		// 		// float  Stairs_Chassis_Speed=0;

		// 		openmv_rx_cpl=0;  	
		// 		openmv_rx_stair=0;
		// 		openmv_rx_stair_dis=0;

		// 		// runActionGroup(0, 1, false);
				
		// 		openmv_send("{R}");
		// 		delay_ms(5);
		// 		openmv_send("{2}");
		// 		delay_ms(5);
		// 		openmv_send("{S}");
		// 		delay_ms(5);


		// 		task=2;
		// 		break;
			
		// 	case  2:
		// 		if (identified==1)
		// 		{			
		// 			if (openmv_rx_cpl == 1)
		// 			{
		// 				sscanf(openmv_rxbuf, "{%d,%d}",&openmv_rx_stair,&openmv_rx_stair_dis);	
		// 				Stairs_Err=openmv_rx_stair_dis-155;
		// 				Stairs_Chassis_Speed=Stairs_Err*0.5;
		// 				if (Stairs_Chassis_Speed>200)Stairs_Chassis_Speed=200;
		// 				if (Stairs_Chassis_Speed<-200)Stairs_Chassis_Speed=-200;
		// 				if (Stairs_Chassis_Speed<0)Stairs_Chassis_Speed=0;
		// 				Chassis_InverseMotionControl(0,Stairs_Chassis_Speed,0);		
		// 				if (Stairs_Err>-5&&Stairs_Err<5)
		// 				{
		// 					Chassis_Stop();
		// 					DelayTask_Add(1,1000,(void (*)(void)) change_flag,"%d%d",&task,3);
		// 					identified=0;
		// 					task=0;
		// 				}
		// 				openmv_rx_cpl=0;
		// 			}
		// 		}
		// 		else
		// 		{
		// 			vl53l0x_start_single_test(&vl53l0x_dev2,&vl53l0x_data);
		// 			if(vl53l0x_data.RangeMilliMeter > 150)               // 冲过头了/楼梯不在
		// 			{
		// 				printf("stair_out");
		// 				Chassis_Stop();	
		// 				while (1);

		// 			}
		// 			if (openmv_rx_cpl == 1)
		// 			{					
		// 				sscanf(openmv_rxbuf, "{%d,%d}",&openmv_rx_stair,&openmv_rx_stair_dis);	
		// 				if(openmv_rx_stair!=0)	
		// 				{
		// 					identified	=1;
		// 				}
		// 				openmv_rx_cpl=0;						
		// 			}
		// 			printf("stair_test");				
		// 			Chassis_InverseMotionControl(0,20,0);						
		// 		}break;	
		// 	case 3:
		// 		Chassis_Stop();
		// 		if (openmv_rx_stair==3)
		// 		{
		// 			runActionGroup(3, 1, false);
		// 		}
		// 		else if (openmv_rx_stair==2)
		// 		{
		// 			runActionGroup(4, 1, false);
		// 		}
		// 		else if (openmv_rx_stair==1)
		// 		{
		// 			runActionGroup(5, 1, false);
		// 		}
		// 		task=4;
		// 		break;
		// 	case 4:
		// 		if(turnplate_flag==1)
		// 		{
		// 			if (Ball_falling() == 0)
		// 			{
		// 				HoleArr[Hole_Idx+3].ball = 0;
		// 				Hole_Idx++;
		// 				ball_num++;
		// 				Turnplate_Move(Hole_Idx);
		// 				Chassis_InverseMotionControl(0,30,0);	
		// 				DelayTask_Add(1,500,(void (*)(void)) change_flag,"%d%d",&turnplate_flag,1);
		// 				DelayTask_Add(1,500,(void (*)(void)) change_flag,"%d%d",&task,2);	
		// 				turnplate_flag = 0;
		// 			}
		// 		}	
		// 		break;	
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
	
	}
}
