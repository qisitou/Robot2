#ifndef INC_24LTCK_OPENMV_H
#define INC_24LTCK_OPENMV_H

#define OPENMV_IRQHandler USART1_IRQHandler
#define OPENMV_UART USART1
#define OPENMV_WriteBuf uart1_WriteBuf

#include "sys.h"

// OpenMV P4 (TX)  ──►  STM32 PA10 (RX)
// OpenMV P5 (RX)  ──►  STM32 PA9  (TX)
// OpenMV GND      ────  STM32 GND


extern u8 openmv_rx_Idx;
extern u8 openmv_rx_cpl;
extern u8 openmv_rx_allow;

extern char openmv_rx_command;
extern int openmv_rx_stair;
extern int openmv_rx_stair_dis;

extern int openmv_rx_stake_dis;


extern char openmv_sendbuf[20];
extern char openmv_rxbuf[20];

void openmv_start(u8 flag);
void openmv_disc();
void openmv_send(char *buf);

#endif //INC_24LTCK_OPENMV_H
