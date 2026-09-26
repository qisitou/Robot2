#include "headfile.h"

////////////////////////////////////参数初始化/////////////////////////////////////

//	APB1/APB1_TIM	42MHz
//	APB2/APB2_TIM	84MHz



//测试openmv发回数据控制底盘	
static void Print_Stairs_Info_Task(void)
{
	printf("%d     ",vl53l0x_data.RangeMilliMeter);
	// printf{""}
}



// uint8_t Hole_Idx_Falling(int8_t Hole_Idx_Falling)
// {
// 	Hole_Idx_Falling=(Hole_Idx+3)%10;
//     if(Hole_Idx_Falling < 0) Hole_Idx_Falling += 10;
// 	return Hole_Idx_Falling;
// }


int main(void)
{
	/*====================初始化====================*/
	LTCK_Init();   
	// DelayTask_Add(1000000, 1000, (void (*)(void))Print_Stairs_Info_Task, NULL);  
	/*==f==================选择红蓝方====================*/
 	Choose_Color();					
 	/*====================前往大转盘====================*/
 	Go_To_Turntable();
	/*====================前往楼梯====================*/
 	Go_To_Stairs();
	/*====================前往小圆盘====================*/
  	Go_To_Small_Turntable();
	/*====================前往仓库====================*/
  	Go_To_Warehouse();
 	/*====================前往====================*/
 	Go_To_Home();

	/*====================测试读卡器====================*/
	// Turnplate_Move(8);
	// delay_ms(wait_time+200);
	// 		HoleArr[0].ball=0;
	// 		HoleArr[1].ball=0;
			// HoleArr[2].ball=0;
			// HoleArr[3].ball=0;
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



	int8_t x=-1;


	uint8_t test=0;
	while (1)
	{
		
		// RingLight_On();
		// delay_ms(2000);
		// RingLight_Off();
		// delay_ms(2000);
			//  if (openmv_rx_cpl == 1)
            // {
            //     /*===================================成功获得六个数=================================*/
            //          /*=================发送的数据是从上到下有球的列数,+该列积木的数字===============*/
            //     if (sscanf(openmv_rxbuf, "{%d,%d,%d,%d,%d,%d}", &layer[0],&openmv_warehouse_block_3,
            //                 &layer[1],&openmv_warehouse_block_2,
            //                 &layer[2],&openmv_warehouse_block_1 ) == 6)
            //     printf("%d,%d,%d,%d,%d,%d",layer[0],openmv_warehouse_block_3,layer[1],openmv_warehouse_block_2,layer[2],openmv_warehouse_block_1);
            //     openmv_rx_cpl = 0;
            // }	  
            // delay_ms(500);
            // // k230_process();
            // // if( k230_rx_ok == 1 )
            // // {
            // //     printf("%d  %d  %d",k230_d1,k230_d2,k230_d3);
            // // }

        


		// if (openmv_rx_cpl == 1)
		// {
		// 	/*===================================成功获得六个数=================================*/
		// 			/*=================发送的数据是从上到下有球的列数,+该列积木的数字===============*/
		// 	if (sscanf(openmv_rxbuf, "{%d,%d,%d,%d,%d,%d}", &layer[0],&openmv_warehouse_block_3,
		// 				&layer[1],&openmv_warehouse_block_2,
		// 				&layer[2],&openmv_warehouse_block_1 ) == 6)
		// 	{
		// 		printf("%d    %d    %d    %d   %d   %d",layer[0],openmv_warehouse_block_3,layer[1],openmv_warehouse_block_2,layer[2],openmv_warehouse_block_1);
		// 	}
		// 	openmv_rx_cpl = 0;
		// }	
		
			
		//测试激光测距
		
		// vl53l0x_start_single_test(&vl53l0x_dev1,&vl53l0x_data);
		// printf("dev1:%5dmm\r\n", vl53l0x_data.RangeMilliMeter);
		// delay_ms(1000);


		// if(1 == openmv_rx_cpl)
		// {
		// 	printf("stop1");
		// 	openmv_rx_command = 0;
		// 	openmv_rx_cpl=0;
		// 	sscanf(openmv_rxbuf,"{%c}",&openmv_rx_command);
		// 	printf("openmv_rx_comm:%c",openmv_rx_command);

		// }


		/*====================转盘读卡器测试====================*/
		// while (test==0)
		// {
		// 	if(turnplate_flag==1)
		// 	{
		// 		if (Ball_falling() == 0)
		// 		{

		// 			HoleArr[Hole_Idx_Falling(Hole_Idx)].ball = 0;
		// 			Hole_Idx++;
		// 			ball_num++;
		// 			Turnplate_Move(Hole_Idx);
		// 			delay_ms(wait_time+300);
		// 			if(5 == ball_num)
		// 			{
		// 				// for(int i=0;i<10;i++)
		// 				// {
		// 				// 	printf("id:%d,ball:%d,ic:%#x\r\n",i,HoleArr[i].ball,HoleArr[i].ic);
		// 				// }
		// 				detect_allow=1;	
		// 				printf("ending1");
		// 				test=1;
		// 			}
		// 			turnplate_flag=0;
		// 			DelayTask_Add(1,400,(void (*)(void)) change_flag,"%d%d",&turnplate_flag,1);
		// 		}
		// 	}
				
		// }
		// while(1);

		// test=0;
		// while (detect_allow==1);
		// delay_ms(100);
		// Hole_Idx=5;
		// Turnplate_Move(Hole_Idx);
		// delay_ms(wait_time+200);

		// // delay_ms(2000);
		// // detect_allow=0;
		// // detect_flag=1;

		// // Hole_Idx=5;
		// // Turnplate_Move(Hole_Idx);
		// // delay_ms(wait_time+200);
 		// while (test==0)
		// {
		// 	if(turnplate_flag==1)
		// 	{
		// 		if (Ball_falling() == 0)
		// 		{

		// 			HoleArr[Hole_Idx_Falling(Hole_Idx)].ball = 0;
		// 			Hole_Idx++;
		// 			ball_num++;
		// 			Turnplate_Move(Hole_Idx);
		// 			if(9 == ball_num)
		// 			{
		// 				// for(int i=0;i<10;i++)
		// 				// {
		// 				// 	printf("id:%d,ball:%d,ic:%#x\r\n",i,HoleArr[i].ball,HoleArr[i].ic);
		// 				// }

		// 				printf("ending2");
		// 				detect_allow=1;
		// 				test=1;	
		// 			}
		// 			turnplate_flag=0;
		// 			DelayTask_Add(1,500,(void (*)(void)) change_flag,"%d%d",&turnplate_flag,1);
		// 		}
		// 	}
				
		// }
		// while (detect_allow==1);
		// printf("ending3");
		// while(1);
		/*====================测试转盘位置====================*/
		// if (Key3_value) // 开始比赛，指示灯变绿
        // {
		// 	x++;			
        //     Turnplate_Move(x);
		// 	rgb_SetColor(RGB_4, GREEN);
		// 	delay_ms(wait_time);
		// 	rgb_SetColor(RGB_4, BLACK);
        //     Key3_value = 0;
        // }
		// if (Key2_value) // 开始比赛，指示灯变绿
        // {
		// 	x--;
        //     Turnplate_Move(x);
		// 	rgb_SetColor(RGB_3, GREEN);
		// 	delay_ms(wait_time);
		// 	rgb_SetColor(RGB_3, BLACK);
        //     Key2_value = 0;
        // }		


		// //成功收到数据
		// k230_process();
		// if( k230_rx_ok == 1 )
		// {
		// 	printf("%d  %d  %d   ",k230_d1,k230_d2,k230_d3);
		// }
		// delay_ms(500);


        // vl53l0x_start_single_test(&vl53l0x_dev1,&vl53l0x_data);
        // printf("vl53l0x_dev2: %d\r\n", vl53l0x_data.RangeMilliMeter);
		// delay_ms(500);
		
		// if(turnplate_flag==1)
		// {
		// 	if (Ball_falling() == 0)
		// 	{

		// 		HoleArr[Hole_Idx+3].ball = 0;
		// 		Hole_Idx++;
		// 		ball_num++;

		// 		if(6 == ball_num)
		// 		{
		// 			for(int i=0;i<10;i++)
		// 			{
		// 				printf("id:%d,ball:%d,ic:%#x\r\n",i,HoleArr[i].ball,HoleArr[i].ic);
		// 				detect_allow=1;	
		// 			}

		// 			printf("ending1");
		// 			while (1);					
		// 		}
		// 		printf("%d",ball_num);
		// 		Turnplate_Move(Hole_Idx);
		// 		delay_ms(wait_time);
		// 		turnplate_flag=0;
		// 		DelayTask_Add(1,500,(void (*)(void)) change_flag,"%d%d",&turnplate_flag,1);
		// 	}
		// }


		//成功,换了个引脚
		// printf("%d",Ball_falling());
		// delay_ms(500);

		/*====================确认可以收到数据和切换任务====================*/
		// if (openmv_rx_cpl == 1)
		// {
		// 	openmv_rx_cpl=0;		
		// 	// printf("task_2");
		// 	openmv_rx_stair=0;					
		// 	sscanf(openmv_rxbuf, "{%d,%d}",&openmv_rx_stair,&openmv_rx_stair_dis);	
		// 	printf("%d",openmv_rx_stair);                  					
		// }		
		// delay_ms(400);

 		// if(openmv_rx_cpl==1)
		// {
		// 	openmv_rx_cpl = 0;
		// 	openmv_rx_command = 0;
		// 	sscanf(openmv_rxbuf,"{%c}",&openmv_rx_command);
		// 	printf("%c",openmv_rx_command);
		// }		
		// else
		// {
		// 	printf("k230_no_rx\r\n");
		// }
		// delay_ms(400);

		

        // k230_process();
        // if (k230_rx_ok == 1)
        // {
		// 	rgb_SetColor(RGB_2, RED);
        // }
		// else if (k230_rx_ok==0)
		// {
		// 	rgb_SetColor(RGB_2, BLUE);
		// }
		// else
		// {
		// 	rgb_SetColor(RGB_3, BLUE);
		// }
		

	
		

	



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




		 /*====================测试灰度====================*/
		// printf("CH1:%d\n\r", GRAY_CH1);
		// printf("CH2:%d\n\r", GRAY_CH2);
		// printf("CH3:%d\n\r", GRAY_CH3);
		// printf("CH4:%d\n\r", GRAY_CH4);
		// printf("GRAY_CH5:%d\n\r", GRAY_CH5);
		// printf("GRAY_CH6:%d\n\r", GRAY_CH6);
		// printf("GRAY_CH7:%d\n\r", GRAY_CH7);
		// delay_ms(1000);

	}
}
