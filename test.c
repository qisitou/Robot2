void Go_To_Warehouse(void)
{

    ///*====================写死===================*/
        // HoleArr[1].ball=0;
        // HoleArr[2].ball=0;
        // HoleArr[3].ball=0;
        // HoleArr[4].ball=0;
        // HoleArr[5].ball=0;
        // HoleArr[6].ball=0;
		// HoleArr[7].ball=0;
        // HoleArr[8].ball=0;
        // HoleArr[9].ball=0;      			
      
        // detect_allow=1;
        // detect_flag=1;
        // delay_ms(50);
        // while (detect_cpl==0);        
		//  HoleArr[0].ic=0x13;	
		 HoleArr[1].ic=0x31;
		 HoleArr[2].ic=0x21;		
		//  HoleArr[3].ic=0x11;
		 HoleArr[4].ic=0x32;
		 HoleArr[5].ic=0x22;
		//  HoleArr[6].ic=0x12;		
		//  HoleArr[7].ic=0x33;
		//  HoleArr[8].ic=0x23;
		//  HoleArr[9].ic=0x13;	




		uint8_t warehouse_row = 0;
		uint8_t warehouse_virtual_col = 0;
        uint8_t warehouse_col = 0;
        uint8_t warehouse_task = 1;
        uint8_t warehouse_task_end = 0;

        uint8_t warehouse_identify_ok = 0; 

        uint8_t block_number= 0;


        int real_col[3];
        uint8_t real_col_num=0;

        delay_ms(10);
        Chassis_AnglePID.Need_Value = 0;       
        Chassis_FixSpeed(20,0,Yaw_Angle,300);        
        Chassis_Stop();
        delay_ms(500);
        Chassis_TurnLeft(90);
        Chassis_AnglePID.Need_Value = 90; //可以试一下放在Chassis_TurnLeft()
        Chassis_GuiWei(90,1000);

        /*====================k230检测数字(仓库顺序)====================*/

		DelayTask_Add(1, 1000, (void (*)(void))change_flag, "%d%d", &overtime_flag_1, 1);
		while (overtime_flag_1==0)
        {
            k230_process();
            if( k230_rx_ok == 1 )
            {
                real_col[0]=k230_d1;//发送数据从左到右发送的
                real_col[1]=k230_d2;
                real_col[2]=k230_d3;
                overtime_flag_1=1;
                warehouse_identify_ok=1;
                printf("%d  %d  %d\n\r",k230_d1,k230_d2,k230_d3);
            }

        }
        if(warehouse_identify_ok!=1)
        {
            // real_col[0]=1;
            // real_col[1]=2;
            // real_col[2]=3;        
            printf("k230timeout");  
            // while(1);
        }
        warehouse_identify_ok=0;





        // /*====================openmv检测数字====================*/
        runActionGroup(9, 1, false);
        arm_finish_waiting();
        openmv_send("{6}");
        openmv_rx_cpl=0;

		DelayTask_Add(1, 1000, (void (*)(void))change_flag, "%d%d", &overtime_flag_2, 1);


        uint8_t openmv_ok_cnt=0;
        int openmv_last[10];
		while (overtime_flag_2==0)
		{

            if (openmv_rx_cpl == 1)
            {
                int d[6];
                if (sscanf(openmv_rxbuf, "{%d,%d,%d,%d,%d,%d}", &d[0],&d[1],&d[2],&d[3],&d[4],&d[5]) == 6)
                {
                    // 和上一帧完全相同 → 次数+1；有不同 → 重新从 1 数
                    if (openmv_ok_cnt > 0 &&
                        d[0]==openmv_last[0] && d[1]==openmv_last[1] && d[2]==openmv_last[2] &&
                        d[3]==openmv_last[3] && d[4]==openmv_last[4] && d[5]==openmv_last[5])
                    {
                        openmv_ok_cnt++;
                    }
                    else
                    {
                        openmv_ok_cnt = 1;
                        for (int i = 0; i < 6; i++) openmv_last[i] = d[i];
                    }

                    if (openmv_ok_cnt >= 3)      // 连续三次一样，才采用
                    {
                        layer[0] = d[0];  openmv_warehouse_block_3 = d[1];
                        layer[1] = d[2];  openmv_warehouse_block_2 = d[3];
                        layer[2] = d[4];  openmv_warehouse_block_1 = d[5];
                        printf("%d,%d,%d,%d,%d,%d\n\r",layer[0],openmv_warehouse_block_3,layer[1],openmv_warehouse_block_2,layer[2],openmv_warehouse_block_1);
                        overtime_flag_2 = 1;
                        warehouse_identify_ok = 1;
                    }
                }
                else
                {
                    openmv_ok_cnt = 0;           // 坏帧，重新数
                    rgb_SetColor(RGB_3, RED);
                }

                openmv_rx_cpl = 0;
            }


            // if (openmv_rx_cpl == 1)
            // {
            //     /*===================================成功获得六个数=================================*/
            //          /*=================发送的数据是从上到下有球的列数,+该列积木的数字===============*/
            //     if (sscanf(openmv_rxbuf, "{%d,%d,%d,%d,%d,%d}", &layer[0],&openmv_warehouse_block_3,
            //                 &layer[1],&openmv_warehouse_block_2,
            //                 &layer[2],&openmv_warehouse_block_1 ) == 6)
            //     {
            //          printf("%d,%d,%d,%d,%d,%d\n\r",layer[0],openmv_warehouse_block_3,layer[1],openmv_warehouse_block_2,layer[2],openmv_warehouse_block_1);
            //         overtime_flag_2=1;
            //         warehouse_identify_ok=1;
            //     }
            //     else
            //     {
            //         rgb_SetColor(RGB_3, RED);
            //     }
                
            //     openmv_rx_cpl = 0;
            // }	
              
		}

        if(warehouse_identify_ok!=1)
        {
            printf("openmvtimeout");  
            // while(1);
            layer[0] = layer[1] = layer[2] = 0;
            openmv_warehouse_block_1 = openmv_warehouse_block_2 = openmv_warehouse_block_3 = 0;            
        }   


        // layer[0]=1; layer[1]=2; layer[2]=3;
        // openmv_warehouse_block_1=1;openmv_warehouse_block_2=2;openmv_warehouse_block_3=3;
		/*====================检测仓库====================*/
        Chassis_AnglePID.Need_Value = 90;
        runActionGroup(14, 1, false);//准备
        arm_finish_waiting();
        Chassis_SINAccel(0,0,0,red_or_blue*(-80),90,100);
        do
        {
            /* code */
            vl53l0x_start_single_test(&vl53l0x_dev1, &vl53l0x_data);
            Chassis_SetSpeed(0, red_or_blue*(-80), Yaw_Angle, 90);
        } while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_LE, 400, 3));
        Chassis_SINAccel(0,red_or_blue*(-80),0,red_or_blue*(-50),90,50);
        do
        {
            /* code */
            vl53l0x_start_single_test(&vl53l0x_dev1, &vl53l0x_data);
            Chassis_SetSpeed(0, red_or_blue*(-50), Yaw_Angle, 90);
        } while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_LE, 250, 3));

        Chassis_AnglePID.Need_Value = 90;
        Chassis_GuiWei(90,1000);
        Chassis_AnglePID.Need_Value = 90;
        Chassis_Stop();   


        vl53l0x_start_single_test(&vl53l0x_dev1, &vl53l0x_data);
        DelayTask_Add(1, 1500, (void (*)(void))change_flag, "%d%d", &flag, 0);
        while (flag)
        {
            vl53l0x_start_single_test(&vl53l0x_dev1, &vl53l0x_data);
            Set_KeepDistance_Y(0, 0, Yaw_Angle, 90, 235);
        }
        flag = 1;
        /*====================定位前准备1:小车移动到离开阶梯的位置====================*/
        delay_ms(10);
        Chassis_SINAccel(0,0,red_or_blue*(40),0,90,100);
        do
        {
            /* code */
            vl53l0x_start_single_test(&vl53l0x_dev1, &vl53l0x_data);
            Chassis_SetSpeed(red_or_blue*(40), 0, Yaw_Angle, 90);
        } while (Range_ConsecutiveMatch_AutoCnt(vl53l0x_data.RangeMilliMeter, CMP_GT, 400, 3));
        Chassis_SINAccel(0,0,red_or_blue*(-40),0,90,300);
        /*====================定位:小车移动到阶梯的位置====================*/
     /*====================定位:小车行驶到第一个缝隙处====================*/
		go_next_gap();
        Chassis_Stop();   
             
            /*================处理仓库=============*/
   		warehouse_row = 3;
        warehouse_col=1;
        warehouse_virtual_col = real_col[real_col_num];
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
                            //抓取仓库的积木,放到车上固定的位置,3,2,1,有53,52,51,43,42,41,33,32,31
                            runActionGroup(((warehouse_row+2)*10+block_number), 1, false);
                            arm_finish_waiting();
                            
                            
                            hole =find_hole( ( (warehouse_row << 4) | warehouse_virtual_col));
                            if(hole!=-1)
                            {
                                Turnplate_Move(hole+3);
                                delay_ms(wait_time+300);
                                runActionGroup(10+warehouse_row, 1, false);//抓取转盘的球放到仓库   13,12,11
                                arm_finish_waiting();                 
                            }           
                            warehouse_row--;                     
                        }
                        else
                        {
                            hole = find_hole(((warehouse_row << 4) | warehouse_virtual_col));
                            if(hole != -1)
                            {
                                Turnplate_Move(hole+3);
                                delay_ms(wait_time+300);              // 等转盘真正转到位
                                runActionGroup(10+warehouse_row, 1, false);
                                arm_finish_waiting();                       // 等机械臂做完(自带10s兜底)
                                HoleArr[hole].ic = 0;
                            }
                            warehouse_row--;
                        }
                    }
                    warehouse_task=2;
                    break;
                case 2:

                    if (real_col_num < 2)      // 还有下一列
                    {
                        warehouse_col++;
                        real_col_num++;
                        go_next_gap();          // 移到下一个缝隙
                        warehouse_virtual_col=real_col[real_col_num];
                        warehouse_row = 3;
                        warehouse_task = 1;
                    }
                    else                        // 三列都处理完了
                    {
                        warehouse_task = 3;
                    }
                    break;
                case 3:
                    go_next_gap();   
                    // // 从车上固定位置(数字3/2/1对应位)放回仓库;识别失败时可能拿空,但不影响完赛
                    // runActionGroup(63, 1, false); arm_finish_waiting();
                    // runActionGroup(62, 1, false); arm_finish_waiting();
                    // runActionGroup(61, 1, false); arm_finish_waiting();                    
                    

                    warehouse_task_end=1;                 
                    break;
                                                                      
            }
            
        }         


    
}