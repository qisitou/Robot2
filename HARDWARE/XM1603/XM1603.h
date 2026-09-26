#ifndef __XM1603_H
#define __XM1603_H

#include "sys.h"
#include <stdbool.h>

#define XM1603_IRQHandler   USART6_IRQHandler
#define XM1603_UART         USART6
#define XM1603_WriteBuf     uart6_WriteBuf

//#define FRAME_HEADER 0x55             //帧头
//#define CMD_SERVO_MOVE 0x03           //舵机移动指令
//#define CMD_ACTION_GROUP_RUN 0x06     //运行动作组指令
//#define CMD_ACTION_GROUP_STOP 0x07    //停止动作组指令
//#define CMD_ACTION_GROUP_SPEED 0x0B   //设置动作组运行速度
//#define CMD_GET_BATTERY_VOLTAGE 0x0F  //获取电池电压指令

#define XM1603_HEADER_1                     0x7E
#define XM1603_HEADER_2                     0x00
#define XM1603_TYPES_READ                   0x07
#define XM1603_TYPES_WRITE                  0x08
#define XM1603_TYPES_SET                    0x09
#define XM1603_LENS_READ                      1
#define XM1603_NOCRC_1                      0xAB
#define XM1603_NOCRC_2                      0xCD

#define XM1603_TX_HEADER_1                    0
#define XM1603_TX_HEADER_2                    1
#define XM1603_TX_TYPES                       2
#define XM1603_TX_LENS                        3
#define XM1603_TX_ADDRESS_HIGH                4
#define XM1603_TX_ADDRESS_LOW                 5

#define XM1603_READ_DATALEN                   6
#define XM1603_READ_CRC_1                     7
#define XM1603_READ_CRC_2                     8


extern u8 QR_allow;
extern u8 QR_code;

void XM1603_Write_Boud(void);
void XM1603_Init(void);
void XM1603_Uart_Init(uint32_t baud);
void XM1603_Write_Byte(u16 Addr,u8 data);


#endif
