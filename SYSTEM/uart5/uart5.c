#include <stdio.h>
#include "uart5.h"

//串口5中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	

//#if IS_CLON == 0
//int __io_putchar(int ch){
//    setbuf(stdout, NULL);
//    while((UART5->SR&0X40)==0);//循环发送,直到发送完毕
//    UART5->DR = (u8) ch;
//    return ch;
//}

//#else

#if UART5_print
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
    while((UART5->SR&0X40)==0);//循环发送,直到发送完毕
    UART5->DR = (u8) ch;
    return ch;
}
#endif

//初始化IO 串口5 
//bound:波特率
void uart5_init(u32 bound){
   //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOC时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE); //使能UART5时钟
 
	//串口5对应引脚复用映射
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5); //GPIOC12复用为UART5
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5); //GPIOD2复用为UART5
	
	//UART5端口配置  PC12-->(TX)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //GPIOC12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化PC12
/*********	PD2-->(RX)  *******/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //GPIOD2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); //初始化PD2

   //UART5 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(UART5, &USART_InitStructure); //初始化串口5
	
    USART_Cmd(UART5, ENABLE);  //使能串口5
	
	//USART_ClearFlag(USART1, USART_FLAG_TC);
	

//	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启相关中断
//
//	//Usart5 NVIC 配置
//    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn ;//串口5中断通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器
}


void uart5_WriteBuf(uint8_t *buf, uint8_t len)
{
	while (len--) {
		while ((UART5->SR & 0x40) == 0);  //SR的第7位置，(USART_FLAG_TC)为1，等待发送结束
		USART_SendData(UART5,*buf++);
		// while (USART_GetFlagStatus(UART5, USART_FLAG_TXE) == RESET);
	}
}
