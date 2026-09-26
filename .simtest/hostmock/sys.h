/*
 * sys.h —— .simtest 宿主机仿真用的桩头文件（覆盖工程的 SYSTEM/sys/sys.h）
 * 目的：让真实的 k230.c 能在 x86 宿主机上编译并运行，从而实测 K230 帧解析逻辑。
 */
#ifndef __SIM_SYS_H
#define __SIM_SYS_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;

#define __IO volatile
#define __I  volatile const
#define __O  volatile

/* ---- 模拟 USART 寄存器块（偏移与真实 STM32 一致，保证真实驱动的位运算行为不变） ---- */
typedef struct
{
    __IO uint32_t SR;   /* 0x00 状态寄存器 */
    __IO uint32_t DR;   /* 0x04 数据寄存器 */
    __IO uint32_t BRR;  /* 0x08 波特率寄存器 */
    __IO uint32_t CR1;  /* 0x0C 控制寄存器1 */
    __IO uint32_t CR2;  /* 0x10 */
    __IO uint32_t CR3;  /* 0x14 */
    __IO uint32_t GTPR; /* 0x18 */
} SIM_USART_TypeDef;

/* 用一个静态实例 + 指针来模拟 "寄存器地址"，避免解引用野指针导致宿主机段错误 */
extern SIM_USART_TypeDef sim_uart6_regs;
extern SIM_USART_TypeDef sim_uart5_regs;
#define USART6 (&sim_uart6_regs)
#define UART5  (&sim_uart5_regs)

/* 真实 STM32 的 SR 位定义 */
#define SIM_USART_SR_RXNE  (1u << 5)
#define SIM_USART_SR_ORE   (1u << 3)
#define SIM_USART_SR_TC    (1u << 6)
#define SIM_USART_SR_TXE   (1u << 7)
#define SIM_USART_SR_IDLE  (1u << 4)

/* ---- 模拟 SPL 的 USART 库函数（只实现驱动真正用到的那几个） ---- */
#define RESET 0
#define SET   1
typedef enum { DISABLE = 0, ENABLE = 1 } FunctionalState;

typedef enum
{
    USART_IT_RXNE = 0x0525,
    USART_IT_ORE  = 0x0325,
    USART_IT_IDLE = 0x0424,
} SIM_USART_IT;
#define USART_IT_RXNE SIM_USART_IT_RXNE
#define USART_IT_ORE  SIM_USART_IT_ORE

#define USART_FLAG_TC SIM_USART_SR_TC

/* 返回值语义与 SPL 一致：SET / RESET */
uint8_t USART_GetITStatus(SIM_USART_TypeDef *USARTx, uint32_t USART_IT);
uint8_t USART_GetFlagStatus(SIM_USART_TypeDef *USARTx, uint32_t USART_FLAG);
uint16_t USART_ReceiveData(SIM_USART_TypeDef *USARTx);
void USART_SendData(SIM_USART_TypeDef *USARTx, uint16_t Data);
void USART_ClearITPendingBit(SIM_USART_TypeDef *USARTx, uint32_t USART_IT);
void USART_ClearFlag(SIM_USART_TypeDef *USARTx, uint32_t USART_FLAG);

/* ---- 仿真辅助接口（仅测试台使用，固件代码不会调用） ---- */
void sim_reset(void);                 /* 复位模拟外设状态与抓包缓冲 */
void sim_feed_rx(const char *bytes);  /* 模拟 K230 从 TX 线上发来一串字节 */
const char *sim_tx_capture(void);     /* 取 STM32 通过 USART6 发出的内容 */

#endif /* __SIM_SYS_H */
