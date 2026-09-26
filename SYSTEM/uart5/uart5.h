#ifndef __UART5_H
#define __UART5_H

#include "stdio.h"
#include "stm32f4xx_conf.h"
#include "sys.h"

/*
 * UART5 —— printf 调试串口
 *   PC12 -> UART5_TX,  PD2 <- UART5_RX
 *
 * UART5_print: 1 = 启用 printf 重定向 (fputc -> UART5), 0 = 关闭
 */
#define UART5_print   1

void uart5_init(u32 bound);
void uart5_WriteBuf(uint8_t *buf, uint8_t len);

#endif
