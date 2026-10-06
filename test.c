void Go_To_Home(void)
{
    Chassis_AnglePID.Need_Value = 90;
    Chassis_MovePath(Chassis_Path_StartToHome);
    Chassis_AnglePID.Need_Value = 90;
    while (1)
    {
        Chassis_GuiWei(90,500);
        while (1)
        {
			// printf("GRAY_CH2:%d\n\r", GRAY_CH2);
			// printf("GRAY_Right:%d\n\r", GRAY_Right);
			// printf("GRAY_behind:%d\n\r", GRAY_behind);
			// printf("GRAY_front:%d\n\r", GRAY_front);	
            u8 f = GRAY_front, r = GRAY_Right, b = GRAY_behind, l = GRAY_CH2; // 0=红 1=黑

            if (f == 1)
            {
				Chassis_SINAccel(0,0,0,red_or_blue*(-40),90,50);
                Chassis_FixSpeed(0, red_or_blue*(-40), 90, 50);
				Chassis_SINAccel(0,red_or_blue*(-40),0,0,90,50);
            }
            if (r == 1)
            {
				Chassis_SINAccel(0,0,-40,0,90,50);
                Chassis_FixSpeed(-40, 0, 90, 50);
				Chassis_SINAccel(-40,0,0,0,90,50);				
            }
            if (b == 1)
            {
				Chassis_SINAccel(0,0,0,red_or_blue*(40),90,50);
                Chassis_FixSpeed(0, red_or_blue*(40), 90, 50);
				Chassis_SINAccel(0,red_or_blue*(40),0,0,90,50);				
            }
            if (l == 1)
            {
				Chassis_SINAccel(0,0,40,0,90,50);
                Chassis_FixSpeed(40, 0, 90, 50);
				Chassis_SINAccel(40,0,0,0,90,50);				
            }
            Chassis_Stop();
            delay_ms(50); // 每次移动后留稳定时间
            if (f == 0 && r == 0 && b == 0 && l == 0)
            {
                break;
            } // 全部为红色，退出循环
        }

		Chassis_SINAccel(0,0,-50,0,90,50);
        while (Range_ConsecutiveMatch_AutoCnt(GRAY_CH2, CMP_EQ, 1, 10))
        {
            delay_ms(10);
            // printf("GRAY_CH1:%d\n\r", GRAY_Right);
			// Chassis_SINAccel(0,0,50,0,0,50);
            Chassis_SetSpeed(-50, 0, Yaw_Angle, 90);
        }
		Chassis_SINAccel(-50,0,50,0,90,100);
        Chassis_FixSpeed(50, 0, 90, 100);
		Chassis_SINAccel(50,0,0,0,90,50);
        Chassis_Stop();

		Chassis_SINAccel(0,0,0,red_or_blue*(50),90,50);
        while (Range_ConsecutiveMatch_AutoCnt(GRAY_front, CMP_EQ, 1, 10))
        {
            delay_ms(10);
            // printf("GRAY_CH1:%d\n\r", GRAY_front);
			// Chassis_SINAccel(0,0,0,50,0,50);
            Chassis_SetSpeed(0, red_or_blue*(50), Yaw_Angle, 90);
        }
		Chassis_SINAccel(0,red_or_blue*(50),0,red_or_blue*(-50),90,100);
        Chassis_FixSpeed(0, red_or_blue*(-50), 90, 150);
		Chassis_SINAccel(0,red_or_blue*(-50),0,0,90,50);
        Chassis_Stop();		

        while (1);
    }
}
