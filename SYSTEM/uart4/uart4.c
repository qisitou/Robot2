#include "uart4.h"	


//注意,读取USARTx->SR能避免莫名其妙的错误   	

//初始化IO 串口4 
//bound:波特率

#if UART4_print
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
	int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
void _sys_exit(int x)
{
	x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
	while((UART4->SR & 0X40)==0);	// 等待DR寄存器空
	UART4->DR = (u8) ch;
	return ch;
}
#endif

void uart4_init(u32 bound){
   //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//使能UART4时钟
 
	//串口4对应引脚复用映射（GPIOA0与GPIOA1）
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_UART4); //GPIOA0复用为UART4
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_UART4); //GPIOA1复用为UART4
	
	//UART4端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //GPIOA0与GPIOA1		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA0，PA1
  

   //UART4 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(UART4, &USART_InitStructure); //初始化串口4
	
    USART_Cmd(UART4, ENABLE);  //使能串口4
	
#if 1
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启相关中断（有接收就中断）
//    USART_ITConfig(UART4, USART_IT_ORE, ENABLE);//开启串口溢出中断

	//Usart4 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn ;//串口4中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
#endif

	//DMA1_Stream2 NVIC 配置（用于CX522读卡器DMA接收）
//    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream2_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
}


void uart4_WriteBuf(uint8_t *buf, uint8_t len)
{
	while (len--)
	{
		while ((UART4->SR & 0x40) == 0)  //SR的第7位置，(USART_FLAG_TC)为1，等待发送结束
        USART_SendData(UART4,*buf++);
	}
}

