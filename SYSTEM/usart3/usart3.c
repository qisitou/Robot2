#include "usart3.h"	
#include "stdio.h"


//#if EN_USART3_RX   //如果使能了接收
//注意,读取USARTx->SR能避免莫名其妙的错误
//加入以下代码,支持printf函数,而不需要选择use MicroLIB

//#if EN_PRINTF_USART3
//#pragma import(__use_no_semihosting)
////标准库需要的支持函数
//struct __FILE
//{
//	int handle;
//};

//FILE __stdout;
////定义_sys_exit()以避免使用半主机模式
//void _sys_exit(int x)
//{
//	x = x;
//}
////重定义fputc函数
//int fputc(int ch, FILE *f)
//{
//	while((USART3->SR & 0X40)==0);	// 等待DR寄存器空
//	USART3->DR = (u8) ch;
//	return ch;
//}
//#endif


u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 USART_RX_STA=0;       //接收状态标记



//初始化IO 串口3 
//bound:波特率
void usart3_init(u32 bound){
   //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟
 
	//串口3对应引脚复用映射
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3); //GPIOD8复用为USART3
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3); //GPIOD9复用为USART3
	
	//USART3端口配置
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; //GPIOD8与GPIOD9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化PD8，PD9

   //USART3 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  	USART_Init(USART3, &USART_InitStructure); //初始化串口3
	
    USART_Cmd(USART3, ENABLE);  //使能串口3 
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	
#if EN_USART3_RX	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启相关中断

	//Usart3 NVIC 配置
  	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//串口3中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3; //抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif
	
}


void uart3_WriteBuf(uint8_t *buf, uint8_t len)
{
	while (len--) {
		while ((USART3->SR & 0x40) == 0);  //SR的第7位置，(USART_FLAG_TC)为1，等待发送结束
//		delay_ms(2);
		USART_SendData(USART3,*buf++);
	}
}

