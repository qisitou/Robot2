#include "uart5.h"

/*
 * UART5 —— printf 调试串口
 *   PC12 -> UART5_TX   (接 USB-TTL 模块的 RX)
 *   PD2  <- UART5_RX   (接 USB-TTL 模块的 TX; 只看打印时可悬空)
 *   GND  <-> GND
 *
 * 波特率由 LTCK_Init() 里的 uart5_init(115200) 设定
 * 时钟域: UART5 挂 APB1 (42MHz)
 */

/*==========================================================
 *  printf 重定向 (fputc)
 *  工程已开 MicroLib (<useUlib>1</useUlib>), 实现 fputc 即可;
 *  __use_no_semihosting 在 MicroLib 下非必需, 且 AC6 不认该 pragma,
 *  故按编译器分别处理, 保证 AC5 / AC6 都能编过。
 *=========================================================*/
#if UART5_print

#if defined(__CC_ARM)
#pragma import(__use_no_semihosting)   /* AC5: 禁止半主机, 避免无调试器时卡死 */
#endif

#if defined(__MICROLIB)
/* MicroLib 自带可用的 __stdout, 不需要自定义 struct __FILE / FILE __stdout */
#else
struct __FILE
{
	int handle;
};
FILE __stdout;
#endif

void _sys_exit(int x)
{
	x = x;
}

int fputc(int ch, FILE *f)
{
	(void)f;
	while ((UART5->SR & 0x40) == 0);   /* 等 TC (SR bit6) 置位, 即上一字节发完 */
	UART5->DR = (u8)ch;
	return ch;
}

#endif /* UART5_print */


/*==========================================================
 *  UART5 接收中断
 *  必须存在: 启动文件 startup_stm32f40_41xxx.s 里 UART5_IRQHandler 是
 *  [WEAK] 默认实现(死循环)。原 K230 占用 UART5 时由 k230.c 提供强符号,
 *  现在 K230 已迁到 USART6, 若这里不补一个实现, 一旦 PD2 收到任何字节
 *  且 RXNE 中断被使能, 就会跳进默认死循环把程序卡死。
 *
 *  本串口用于 printf 输出, 接收数据仅丢弃(顺手清 ORE), 不做协议解析。
 *  若以后要用 UART5 收数据, 把下面的 body 换成自己的解析逻辑。
 *
 *  注意: uart5_init() 默认【不】打开 RXNE 中断, 所以正常情况下这个
 *        函数不会被触发, 它只是兜底。需要收数据时自行取消注释那两行。
 *=========================================================*/
void UART5_IRQHandler(void)
{
	if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{
		(void)USART_ReceiveData(UART5);   /* 读 DR, 清 RXNE; 打印串口收到的数据直接丢弃 */
	}

	if (USART_GetITStatus(UART5, USART_IT_ORE) != RESET)
	{
		USART_ClearITPendingBit(UART5, USART_IT_ORE);
		(void)USART_ReceiveData(UART5);   /* 清溢出状态 */
	}
}


//初始化IO 串口5
//bound:波特率
void uart5_init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); //使能GPIOC时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); //使能GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); //使能UART5时钟

	//UART5引脚复用映射
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5); //PC12复用为UART5_TX
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource2,  GPIO_AF_UART5); //PD2 复用为UART5_RX

	//UART5端口配置  PC12-->(TX)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;   //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;   //上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);         //初始化PC12

	//UART5端口配置  PD2-->(RX)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOD, &GPIO_InitStructure);         //初始化PD2

	//UART5 初始化设置
	USART_InitStructure.USART_BaudRate = bound;                              //波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;              //字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;                   //一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;                      //无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;          //收发模式
	USART_Init(UART5, &USART_InitStructure);        //初始化串口5

	USART_Cmd(UART5, ENABLE);                       //使能串口5
	USART_ClearFlag(UART5, USART_FLAG_TC);

	/* ---- 接收中断: 本串口只做 printf 输出, 默认不打开 ----
	 * 需要 UART5 收数据时, 取消下面两段的注释即可 (ISR 已备好, 会安全丢弃/或改成你的解析) */
	// USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);

	// NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	// NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  // 最低, 不干扰 500Hz 电机中断
	// NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	// NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// NVIC_Init(&NVIC_InitStructure);
	(void)NVIC_InitStructure;   /* 未启用接收中断时避免 unused 警告 */
}


void uart5_WriteBuf(uint8_t *buf, uint8_t len)
{
	while (len--) {
		while ((UART5->SR & 0x40) == 0);
		USART_SendData(UART5, *buf++);
	}
}
