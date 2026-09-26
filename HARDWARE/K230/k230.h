#ifndef INC_ROBOT2_K230_H
#define INC_ROBOT2_K230_H

#include "sys.h"

#define K230_IRQHandler USART6_IRQHandler
#define K230_UART       USART6
#define K230_WriteBuf   uart6_WriteBuf
#define K230_RX_BUF_LEN 64

extern volatile u8 k230_rx_Idx;
extern volatile u8 k230_rx_cpl;
extern volatile u8 k230_rx_ok;
extern int k230_x1, k230_y1, k230_num1;
extern int k230_x2, k230_y2, k230_num2;
extern int k230_d1, k230_d2, k230_d3;    // 任务6：三个数字拼成一帧
extern char k230_rxbuf[K230_RX_BUF_LEN];

void k230_send(char *buf);
void k230_send_command(u8 command);
void k230_process(void);

#endif
