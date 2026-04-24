#include "headfile.h"                  // Device header


typedef struct
{
	uint8_t _Flag;		//修改标志位	0-目标状态被修改 1-目标状态未修改
	
	int8_t Dir;			//方向			0-逆时针 1-顺时针
	uint16_t Speed;		//速度
	uint8_t Acc;		//加速度
	uint32_t Step;		//步数
}StepMotor_Struct;//步进目标状态结构体

StepMotor_Struct StepMotor_Target[4];//步进目标状态

/*
 *函数简介:步进张大头使能控制指令
 *参数说明:ID		ID
 *返回类型:无
 *备注:默认不进行多机同步
 *指令:
 *		ID 0xF3 0xAB En Sync 0x6B
 *	  ID	地址
 *	  En	使能状态 		0-失能 1-使能
 *	  Sync	多机同步标志 	0-不多机同步 1-多机同步
 */
void StepMotor_ZDTENABLE(uint8_t ID)
{	
	uint8_t cmd[16] = {0};

	// 装载命令
	cmd[0]=ID;		 // 地址
	cmd[1]=0xF3;	 // 功能码
	cmd[2]=0xAB;	// 辅助码
	cmd[3]=0x01;	// 使能状态
	cmd[4]=0x00;	// 多机同步运动标志
	cmd[5]=0x6B;	// 校验字节

	// 发送命令
  	can_SendCmd(cmd, 6);
}

/*
 *函数简介:步进张大头速度模式控制指令
 *参数说明:ID		ID
 *参数说明:Dir		方向			0-逆时针 1-顺时针
 *参数说明:Speed	速度			范围0 - 5000RPM
 *参数说明:Acc		加速度			 0表示无加减速
 *参数说明:Sync		多机同步标志	0-不多机同步 1-多机同步
 *返回类型:无
 *备注:无
 *指令:
 *		ID 0xF6 Dir Speed_H Speed_L Acc Sync 0x6B
 *	  ID		地址
 *	  Dir		方向			0-逆时针 1-顺时针
 *	  Speed_H	速度高四位		单位RPM
 *	  Speed_L	速度低四位		单位RPM
 *	  Acc		加速度			0表示无加减速
 *	  Sync		多机同步标志	0-不多机同步 1-多机同步
 */
void StepMotor_ZDTSetSpeed(uint8_t ID, uint8_t dir, uint16_t vel, uint8_t acc, bool snF)
{
	uint8_t cmd[16] = {0};

	// 装载命令
	cmd[0] =  ID;                       // 地址
	cmd[1] =  0xF6;                       // 功能码
	cmd[2] =  dir;                        // 方向
	cmd[3] =  (uint8_t)(vel >> 8);        // 速度(RPM)高8位字节
	cmd[4] =  (uint8_t)(vel >> 0);        // 速度(RPM)低8位字节
	cmd[5] =  acc;                        // 加速度，注意：0是直接启动
	cmd[6] =  snF;                        // 多机同步运动标志
	cmd[7] =  0x6B;                       // 校验字节
	
	// 发送命令
	can_SendCmd(cmd, 8);
}

/*
 *函数简介:步进张大头位置模式控制指令
 *参数说明:ID		ID
 *参数说明:Dir		方向			0-逆时针 1-顺时针
 *参数说明:Speed	速度			单位RPM
 *参数说明:Acc		加速度			0表示无加减速
 *参数说明:Step		脉冲数
 *参数说明:Mode		模式标志		0-相对位置模式 1-绝对位置模式
 *参数说明:Sync		多机同步标志	0-不多机同步 1-多机同步
 *返回类型:无
 *备注:无
 *指令:
 *		ID 0xFD Dir Speed_H Speed_L Acc Step[31:23] Step[23:16] Step[15:8] Step[7:0] Mode Sync 0x6B
 *	  ID			地址
 *	  Dir			方向			0-逆时针 1-顺时针
 *	  Speed_H		速度高四位		单位RPM
 *	  Speed_L		速度低四位		单位RPM
 *	  Acc			加速度			0表示无加减速
 *	  Step[31:23]	脉冲数[31:23]
 *	  Step[23:16]	脉冲数[23:16]
 *	  Step[15:8]	脉冲数[15:8]
 *	  Step[7:0]		脉冲数[7:0]
 *	  Mode			模式标志		0-相对位置模式 1-绝对位置模式
 *	  Sync			多机同步标志	0-不多机同步 1-多机同步
 */
void StepMotor_ZDTSetPosition(uint8_t ID, uint8_t dir, uint16_t vel, uint8_t acc, uint32_t clk, bool raF, bool snF)
{
	uint8_t cmd[16] = {0};

	// 装载命令
	cmd[0]  =  ID;                      // 地址
	cmd[1]  =  0xFD;                      // 功能码
	cmd[2]  =  dir;                       // 方向
	cmd[3]  =  (uint8_t)(vel >> 8);       // 速度(RPM)高8位字节
	cmd[4]  =  (uint8_t)(vel >> 0);       // 速度(RPM)低8位字节 
	cmd[5]  =  acc;                       // 加速度，注意：0是直接启动
	cmd[6]  =  (uint8_t)(clk >> 24);      // 脉冲数(bit24 - bit31)
	cmd[7]  =  (uint8_t)(clk >> 16);      // 脉冲数(bit16 - bit23)
	cmd[8]  =  (uint8_t)(clk >> 8);       // 脉冲数(bit8  - bit15)
	cmd[9]  =  (uint8_t)(clk >> 0);       // 脉冲数(bit0  - bit7 )
	cmd[10] =  raF;                       // 相位/绝对标志，false为相对运动，true为绝对值运动
	cmd[11] =  snF;                       // 多机同步运动标志，false为不启用，true为启用
	cmd[12] =  0x6B;                      // 校验字节
	
	// 发送命令
	can_SendCmd(cmd, 13);
}

/*
 *函数简介:步进张大头多机同步运动指令
 *参数说明:ID		ID
 *返回类型:无
 *备注:无
 *指令:
 *		ID 0xFF 0x66 0x6B
 *	  ID		地址 0表示广播地址
 */
void StepMotor_ZDTSyncMove(uint8_t ID)
{
	uint8_t cmd[16] = {0};
	
	// 装载命令
	cmd[0] =  ID;                       // 地址
	cmd[1] =  0xFF;                       // 功能码
	cmd[2] =  0x66;                       // 辅助码
	cmd[3] =  0x6B;                       // 校验字节
	
	// 发送命令
	can_SendCmd(cmd, 4);
}

/*
 *函数简介:步进张大头立即停止指令
 *参数说明:ID		ID
 *参数说明:Sync		多机同步标志 0-不多机同步 1-多机同步
 *返回类型:无
 *备注:无
 *指令:
 *		ID 0xFE 0x98 Sync 0x6B
 *	  ID		地址
 *	  Sync		多机同步标志 0-不多机同步 1-多机同步
 */
void StepMotor_ZDTStop(uint8_t ID,uint8_t snF)
{
	uint8_t cmd[16] = {0};
	
	// 装载命令
	cmd[0] =  ID;                       // 地址
	cmd[1] =  0xFE;                       // 功能码
	cmd[2] =  0x98;                       // 辅助码
	cmd[3] =  snF;                        // 多机同步运动标志
	cmd[4] =  0x6B;                       // 校验字节
	
	// 发送命令
	can_SendCmd(cmd, 5);
}

/*
 *函数简介:步进初始化
 *参数说明:无
 *返回类型:无
 *备注:在使能步进之后才会启动定时器TIM6,防止使能帧被定时器中断打断
 */
void StepMotor_Init(void)
{	
	StepMotor_ZDTENABLE(1);delay_us(500);
	StepMotor_ZDTENABLE(2);delay_us(500);
	StepMotor_ZDTENABLE(3);delay_us(500);
	StepMotor_ZDTENABLE(4);delay_us(500);
	TIM_Cmd(TIM5,ENABLE);
}

/*
 *函数简介:步进设置速度
 *参数说明:ID		ID
 *参数说明:Speed	速度	单位RPM
 *返回类型:无
 *备注:默认采用多机同步,无加减速
 *备注:会设置StepMotor_Target结构体来调整DMA发送值
 */
void StepMotor_SetSpeed(uint8_t ID,int32_t Speed)
{
	if(Speed>=0)
	{
		StepMotor_Target[ID-1].Dir=0;
		StepMotor_Target[ID-1].Speed=Speed;
	}
	else
	{
		StepMotor_Target[ID-1].Dir=1;
		StepMotor_Target[ID-1].Speed=-Speed;
	}

	StepMotor_Target[ID-1]._Flag=0;
}

/*
 *函数简介:步进设置位置
 *参数说明:ID		ID
 *参数说明:Speed	速度	单位RPM
 *参数说明:Step		脉冲数
 *返回类型:无
 *备注:默认采用逆时针方向,无加减速,绝对位置模式,不采用多机同步
 *备注:会设置StepMotor_Target结构体来调整DMA发送值
 */
void StepMotor_SetPosition(uint8_t ID,uint16_t Speed,uint32_t Step)
{
	StepMotor_Target[ID-1].Speed=Speed;
	StepMotor_Target[ID-1].Acc=0;
	StepMotor_Target[ID-1].Step=Step;
	
	StepMotor_Target[ID-1]._Flag=0;
}

/*
 *函数简介:步进设置位置(扩展)
 *参数说明:ID		ID
 *参数说明:Speed	速度	单位RPM
 *参数说明:Step		脉冲数
 *返回类型:无
 *备注:默认采用逆时针方向,无加减速,绝对位置模式,不采用多机同步
 *备注:会设置StepMotor_Target结构体来调整DMA发送值
 */
void StepMotor_SetPositionExt(uint8_t ID,uint16_t Speed,uint8_t Acc,uint32_t Step)
{
	StepMotor_Target[ID-1].Speed=Speed;
	StepMotor_Target[ID-1].Acc=Acc;
	StepMotor_Target[ID-1].Step=Step;
	
	StepMotor_Target[ID-1]._Flag=0;
}

/*
 *函数简介:步进定时发送回调函数
 *参数说明:无
 *返回类型:无
 *备注:在TIM5定时中断调用
 *备注:通过计数器循环发送各指令
 */
void TIM5_IRQHandler(void)//1.5ms
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //检查TIM5更新中断发生与否
	{
        static uint8_t Count=0;
		Count=(Count+1)%6;
	
		switch(Count)
		{
			case 0:
				if(StepMotor_Target[0]._Flag==0)
				{
					StepMotor_Target[0]._Flag=1;
					StepMotor_ZDTSetSpeed(1,StepMotor_Target[0].Dir,StepMotor_Target[0].Speed,0,1);
				}
				break;
			case 1:
				if(StepMotor_Target[1]._Flag==0)
				{
					StepMotor_Target[1]._Flag=1;
					StepMotor_ZDTSetSpeed(2,StepMotor_Target[1].Dir,StepMotor_Target[1].Speed,0,1);
				}
				break;
			case 2:
				if(StepMotor_Target[2]._Flag==0)
				{
					StepMotor_Target[2]._Flag=1;
					StepMotor_ZDTSetSpeed(3,StepMotor_Target[2].Dir,StepMotor_Target[2].Speed,0,1);
				}
				break;
			case 3:
				if(StepMotor_Target[3]._Flag==0)
				{
					StepMotor_Target[3]._Flag=1;
					StepMotor_ZDTSetSpeed(4,StepMotor_Target[3].Dir,StepMotor_Target[3].Speed,0,1);
				}
				break;
			case 4:StepMotor_ZDTSyncMove(0);break;
			case 5:break;
		}
    
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);  //清除TIMx更新中断标志

	}

}
