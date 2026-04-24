#include "can.h"
#include "stdio.h"

/**
	* @brief   初始化CAN
	* @param   无
	* @retval  无
**/

void can_init(void)
{
	
	GPIO_InitTypeDef 	   GPIO_InitStructure; 
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;			// 过滤器结构体

#if CAN_RX
   	NVIC_InitTypeDef       NVIC_InitStructure;
#endif

	// 设置STM32的帧ID - 扩展帧格式 - 不过滤任何数据帧
	__IO uint8_t id_o, im_o; __IO uint16_t id_l, id_h, im_l, im_h;
	id_o = (0x00);
	id_h = (uint16_t)((uint16_t)id_o >> 5);				  // 高3位
	id_l = (uint16_t)((uint16_t)id_o << 11) | CAN_ID_EXT; // 低5位
	im_o = (0x00);
	im_h = (uint16_t)((uint16_t)im_o >> 5);
	im_l = (uint16_t)((uint16_t)im_o << 11) | CAN_ID_EXT;
    
	//使能相关时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能PORTA时钟	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	
	
    //初始化GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化PA11,PA12
	
	//引脚复用映射配置
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1); //GPIOA11复用为CAN1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1); //GPIOA12复用为CAN1
	  
  	//CAN配置的波特率是 500 kbps
   	CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	//软件自动离线管理	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure.CAN_NART=ENABLE;	//使能报文自动传送 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定 
  	CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;	 //模式设置 
  	CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
  	CAN_InitStructure.CAN_BS1=CAN_BS1_4tq; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
  	CAN_InitStructure.CAN_BS2=CAN_BS2_1tq;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
  	CAN_InitStructure.CAN_Prescaler=14;  //分频系数(Fdiv)为brp+1	
  	CAN_Init(CAN1, &CAN_InitStructure);   // 初始化CAN1 
	
		
	//配置过滤器
 	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //过滤器0
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; // 掩码模式
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure.CAN_FilterIdHigh=id_h;	// 过滤器标识符的高16位值
  	CAN_FilterInitStructure.CAN_FilterIdLow=id_l;	// 过滤器标识符的低16位值
  	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=im_h;	// 过滤器屏蔽标识符的高16位值
  	CAN_FilterInitStructure.CAN_FilterMaskIdLow=im_l;	// 过滤器屏蔽标识符的低16位值
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //使能过滤器
  	CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
		
#if CAN_RX
	CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);				//FIFO0消息挂号中断允许.		    
  
  	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);
#endif
}

__IO CAN_t can = {0};

/**
	* @brief   CAN1_RX0接收中断
	* @param   无
	* @retval  无
*/
#if CAN_RX
uint8_t Can_Reach_Count = 0;
uint8_t Motor_Num = 0;
void CAN1_RX0_IRQHandler(void)
{
	// 接收一包数据
	CAN_Receive(CAN1, CAN_FIFO0, (CanRxMsg *)(&can.CAN_RxMsg));
	can.rxFrameFlag = true;
	if (can.CAN_RxMsg.DLC >= 3)
	{
		uint8_t addr = can.CAN_RxMsg.ExtId >> 8;
		uint8_t cmd  = can.CAN_RxMsg.Data[0];
		uint8_t sub  = can.CAN_RxMsg.Data[1];
		uint8_t chk  = can.CAN_RxMsg.Data[2];

		if ((cmd == 0xFD||cmd == 0xF6||cmd == 0xFE) && sub == 0x02 && chk == 0x6B)	//位置模式/速度模式/立即停止
		{
			Motor_Num = addr;
			//printf("Motor 0x%02X get.\n", addr);
		}
		else if (cmd == 0xFF && sub == 0x02 && chk == 0x6B)//多机同步
		{
			Motor_Num = 0;
			//printf("多机同步.\n");
		}
		else if (cmd == 0xFD && sub == 0x9F && chk == 0x6B)//位置模式到位
		{
			Can_Reach_Count++;
//			printf("Motor 0x%02X reach position.\n", addr);
		}
	}
}

#endif


/**
	* @brief   CAN发送多个字节
	* @param   无
	* @retval  无
	*/
void can_SendCmd(__IO uint8_t *cmd, uint8_t len)
{
	__IO uint8_t i = 0, j = 0, k = 0, l = 0, packNum = 0;

	// 除去ID地址和功能码后的数据长度
	j = len - 2;

	// 发送数据
	while(i < j)
	{
		// 数据个数
		k = j - i;

		// 填充缓存
		can.CAN_TxMsg.StdId = 0x00;
		can.CAN_TxMsg.ExtId = ((uint32_t)cmd[0] << 8) | (uint32_t)packNum;
		can.CAN_TxMsg.Data[0] = cmd[1];
		can.CAN_TxMsg.IDE = CAN_Id_Extended;
		can.CAN_TxMsg.RTR = CAN_RTR_Data;

		// 小于8字节命令
		if(k < 8)
		{
			for(l=0; l < k; l++,i++) { can.CAN_TxMsg.Data[l + 1] = cmd[i + 2]; } can.CAN_TxMsg.DLC = k + 1;
		}
		// 大于8字节命令，分包发送，每包数据最多发送8个字节
		else
		{
			for(l=0; l < 7; l++,i++) { can.CAN_TxMsg.Data[l + 1] = cmd[i + 2]; } can.CAN_TxMsg.DLC = 8;
		}

		// 发送数据
		//CAN_Transmit(CAN1, (CanTxMsg *)(&can.CAN_TxMsg));
		uint8_t mailbox = CAN_Transmit(CAN1, (CanTxMsg *)(&can.CAN_TxMsg));
		if (mailbox != 3)
		{
			// 等待发送完成
			while ((CAN1->TSR & (CAN_TSR_RQCP0 << mailbox)) == 0);
		}
		
		// 记录发送的第几包的数据
		++packNum;
	}
}

