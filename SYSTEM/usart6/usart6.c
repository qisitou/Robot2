#include "sys.h"
#include "usart6.h"


//初始化IO 串口6
//bound:波特率
//本串口原用于 XM1603 扫码模块(已移除), 现改为 K230 视觉模块通信口(替代原 UART5)
//接线: STM32 PG14 (USART6_TX) -> K230 RX
//      STM32 PG9  (USART6_RX) <- K230 TX
//      STM32 GND              <-> K230 GND
void usart6_init(u32 bound){
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);   //使能GPIOG时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);  //使能USART6时钟(USART6挂APB2)

	//串口6对应引脚复用映射
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6);  //PG9 复用为USART6_RX
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6); //PG14复用为USART6_TX


	//USART6端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14; //PG9(TX)与PG14(RX)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOG,&GPIO_InitStructure); //初始化PG9，PG14

   //USART6 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART6, &USART_InitStructure); //初始化串口6


    USART_Cmd(USART6, ENABLE);  //使能串口6
    USART_ClearFlag(USART6, USART_FLAG_TC);  //清除发送完成标志位


#if EN_USART6_RX
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//开启相关中断

	//USART6 NVIC 配置(与原UART5保持一致: 抢占1/子2)
    NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//串口6中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化NVIC寄存器、

#endif

}





void uart6_WriteBuf(uint8_t *buf, uint8_t len)
{
	while (len--) {
		while ((USART6->SR & 0x40) == 0);  //SR的第7位置，(USART_FLAG_TC)为1，等待发送结束
		USART_SendData(USART6,*buf++);
	}
}
