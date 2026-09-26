#include "k230.h"
#include "usart6.h"
#include <stdio.h>

volatile u8 k230_rx_Idx;
volatile u8 k230_rx_cpl = 0;
volatile u8 k230_rx_ok = 0;

int k230_x1, k230_y1, k230_num1;
int k230_x2, k230_y2, k230_num2;
int k230_d1, k230_d2, k230_d3;
char k230_rxbuf[K230_RX_BUF_LEN];

void k230_send(char *buf)
{
    K230_WriteBuf((u8 *)buf, 3);
}

void k230_send_command(u8 command)
{
    char sendbuf[4];

    if (command > 9)
    {
        return;
    }
    sendbuf[0] = '{';
    sendbuf[1] = (char)('0' + command);
    sendbuf[2] = '}';
    sendbuf[3] = '\0';
    k230_send(sendbuf);
}

void k230_process(void)
{
    if (k230_rx_cpl == 1)
    {
        // 任务6:三个数字拼成一帧,如{123}
        if (sscanf(k230_rxbuf, "{%1d%1d%1d}", &k230_d1, &k230_d2, &k230_d3) == 3
            && k230_d1 != k230_d2
            && k230_d1 != k230_d3
            && k230_d2 != k230_d3)
        {
            k230_rx_ok = 1;
        }
        else
        {
            k230_rx_ok = 0;
        }

        k230_rx_cpl = 0;
    }
}

void K230_IRQHandler(void)
{
    if (USART_GetITStatus(K230_UART, USART_IT_RXNE) != RESET)
    {
        u8 Res = K230_UART->DR;

        if ('{' == Res)
        {
            k230_rx_cpl = 0;
            k230_rx_ok = 0;
            k230_rx_Idx = 0;
            k230_rxbuf[k230_rx_Idx] = '{';
            return;
        }

        if ('}' == Res && k230_rxbuf[0] == '{')
        {
            if (k230_rx_Idx < K230_RX_BUF_LEN - 2)
            {
                k230_rx_Idx++;
                k230_rxbuf[k230_rx_Idx] = '}';
                k230_rxbuf[k230_rx_Idx + 1] = '\0';
                k230_rx_Idx = 0;
                k230_rx_cpl = 1;
            }
            else
            {
                k230_rx_Idx = 0;
                k230_rxbuf[0] = 0;
            }
        }
        else if (k230_rx_Idx < K230_RX_BUF_LEN - 2 && k230_rxbuf[0] == '{')
        {
            k230_rx_Idx++;
            k230_rxbuf[k230_rx_Idx] = Res;
        }
        else
        {
            k230_rx_Idx = 0;
            k230_rxbuf[0] = 0;
        }
    }

    if (USART_GetITStatus(K230_UART, USART_IT_ORE) != RESET)
    {
        USART_ClearITPendingBit(K230_UART, USART_IT_ORE);
        (void)USART_ReceiveData(K230_UART);
        k230_rx_Idx = 0;
        k230_rxbuf[0] = 0;
    }
}
