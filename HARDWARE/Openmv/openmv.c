#include "openmv.h"
#include "usart1.h"

u8 openmv_rx_Idx;
u8 openmv_rx_cpl = 0;
u8 openmv_rx_allow = 1;

char openmv_rx_command;
int openmv_rx_stair;
int openmv_rx_stair_dis;

int openmv_rx_stake_dis;

char openmv_sendbuf[20];
char openmv_rxbuf[20];

void openmv_start(u8 flag)
{
    // if(TASK_RED == flag)
    // {
    //     openmv_send("{R}");
    // }
    // else if(TASK_BLUE == flag)
    // {
    //     openmv_send("{B}");
    // }
}

void openmv_send(char *buf)
{
    sprintf(openmv_sendbuf, buf);
    OPENMV_WriteBuf((u8 *)openmv_sendbuf,3);
}

u32 U4_cnt = 0;
u32 U4_flag = 0;

void OPENMV_IRQHandler(void)                	//串口1中断服务程序
{
//    U4_cnt++;
//    U4_flag++;

    if(USART_GetITStatus(OPENMV_UART, USART_IT_RXNE) != RESET)  //接收中断
    {
//        u8 Res = USART_ReceiveData(OPENMV_UART);//(USART1->DR);	//读取接收到的数据
        u8 Res = OPENMV_UART->DR;//(USART1->DR);	//读取接收到的数据

//		printf("%#x ",Res);					//经过 %d 译码过了    //容易嵌套中断，导致卡死
//		printf("%c",Res);					//经过 %d 译码过了    //容易嵌套中断，导致卡死

        if('{' == Res)
        {
            openmv_rx_cpl = 0;
            openmv_rx_Idx = 0;
            openmv_rxbuf[openmv_rx_Idx] = '{';

            U4_flag--;
            return;
        }

        if(('}' == Res) && (openmv_rxbuf[0] == '{'))
        {
            openmv_rx_Idx++;
            openmv_rxbuf[openmv_rx_Idx] = '}';
            openmv_rx_Idx = 0;
            openmv_rx_cpl = 1;
        }
        else
        {
            openmv_rx_Idx++;
            openmv_rxbuf[openmv_rx_Idx] = Res;
        }

    }

    if(USART_GetITStatus(OPENMV_UART, USART_IT_ORE) != RESET)
    {
        USART_ClearITPendingBit(OPENMV_UART, USART_IT_ORE);
        (void)USART_ReceiveData(OPENMV_UART);  //清除溢出状态
        openmv_rxbuf[0] = 0;
    }

//    U4_flag--;
}
