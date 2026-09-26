/*
 * test_k230.c —— K230 通信链路功能仿真测试台
 *
 * 做法：本文件直接编译真实的 HARDWARE/K230/k230.c，
 *       把 USART6 换成一个行为等价的模拟寄存器块，
 *       再由测试台模拟"K230 从串口发来一帧数据"，
 *       通过调用真实的中断服务函数来驱动真实的解析状态机。
 *
 * 这样验证的是迁移后 USART6 通道上的实际收发行为，而不是重写一遍逻辑。
 */
#include "sys.h"
#include "k230.h"

/* ============================ 模拟外设实现 ============================ */

SIM_USART_TypeDef sim_uart6_regs;
SIM_USART_TypeDef sim_uart5_regs;

#define TX_CAP_LEN 256
static char     g_tx_cap[TX_CAP_LEN];
static unsigned g_tx_len;

void sim_reset(void)
{
    memset(&sim_uart6_regs, 0, sizeof(sim_uart6_regs));
    memset(&sim_uart5_regs, 0, sizeof(sim_uart5_regs));
    memset(g_tx_cap, 0, sizeof(g_tx_cap));
    g_tx_len = 0;
}

uint8_t USART_GetITStatus(SIM_USART_TypeDef *USARTx, uint32_t USART_IT)
{
    switch (USART_IT)
    {
    case SIM_USART_IT_RXNE: return (USARTx->SR & SIM_USART_SR_RXNE) ? SET : RESET;
    case SIM_USART_IT_ORE:  return (USARTx->SR & SIM_USART_SR_ORE) ? SET : RESET;
    default:                return RESET;
    }
}

uint8_t USART_GetFlagStatus(SIM_USART_TypeDef *USARTx, uint32_t USART_FLAG)
{
    return (USARTx->SR & USART_FLAG) ? SET : RESET;
}

uint16_t USART_ReceiveData(SIM_USART_TypeDef *USARTx)
{
    uint16_t d = (uint16_t)(USARTx->DR & 0x1FF);
    USARTx->SR &= ~SIM_USART_SR_RXNE; /* 读 DR 自动清 RXNE，与真实硬件一致 */
    return d;
}

void USART_SendData(SIM_USART_TypeDef *USARTx, uint16_t Data)
{
    (void)USARTx;
    (void)Data;
}

void USART_ClearITPendingBit(SIM_USART_TypeDef *USARTx, uint32_t USART_IT)
{
    if (USART_IT == SIM_USART_IT_ORE) { USARTx->SR &= ~SIM_USART_SR_ORE; }
}

void USART_ClearFlag(SIM_USART_TypeDef *USARTx, uint32_t USART_FLAG)
{
    USARTx->SR &= ~USART_FLAG;
}

/* 模拟"K230 发来一串字节"：逐字节置 RXNE 并调用真实的中断服务函数 */
void sim_feed_rx(const char *bytes)
{
    for (const char *p = bytes; *p; ++p)
    {
        sim_uart6_regs.DR = (uint8_t)*p;
        sim_uart6_regs.SR |= SIM_USART_SR_RXNE;
        USART6_IRQHandler(); /* ← 真实 ISR（由 k230.h 的宏映射而来） */
    }
}

const char *sim_tx_capture(void) { return g_tx_cap; }

/* ==================== 被测固件依赖的最小桩函数 ==================== */

/* k230.c 用 K230_WriteBuf -> uart6_WriteBuf 发送，这里抓包 */
void uart6_WriteBuf(uint8_t *buf, uint8_t len)
{
    for (uint8_t i = 0; i < len && g_tx_len < TX_CAP_LEN - 1; ++i)
    {
        g_tx_cap[g_tx_len++] = (char)buf[i];
    }
    g_tx_cap[g_tx_len] = '\0';
}

/* ============================== 测试用例 ============================== */

static int g_pass = 0, g_fail = 0;

static void check(const char *name, int ok, const char *detail)
{
    if (ok) { ++g_pass; printf("  [PASS] %s\n", name); }
    else    { ++g_fail; printf("  [FAIL] %s   <- %s\n", name, detail); }
}

int main(void)
{
    printf("=========================================================\n");
    printf(" K230 on USART6 - 功能仿真 (真实 k230.c + 模拟 USART6 寄存器)\n");
    printf("=========================================================\n");

    /* ---- 用例1：单数字指令帧 {6} 的接收与解析 ---- */
    sim_reset();
    sim_feed_rx("{6}");
    check("1.1 收到完整帧后 k230_rx_cpl 置 1", k230_rx_cpl == 1,
          "k230_rx_cpl != 1");
    check("1.2 帧缓冲内容为 \"{6}\"", strcmp(k230_rxbuf, "{6}") == 0,
          k230_rxbuf);
    k230_process();
    check("1.3 k230_process 解析出 d1=6", k230_d1 == 6, "d1 != 6");
    check("1.4 k230_rx_ok 置 1", k230_rx_ok == 1, "k230_rx_ok != 1");
    check("1.5 k230_rx_cpl 被消费清零", k230_rx_cpl == 0, "k230_rx_cpl != 0");

    /* ---- 用例2：任务6 的三数字帧 {123} ---- */
    sim_reset();
    sim_feed_rx("{123}");
    k230_process();
    check("2.1 三数字帧解析 d1=1,d2=2,d3=3",
          k230_d1 == 1 && k230_d2 == 2 && k230_d3 == 3,
          "三数字解析错误");
    check("2.2 k230_rx_ok 置 1", k230_rx_ok == 1, "k230_rx_ok != 1");

    /* ---- 用例3：发送方向 {n} 指令，验证走的是 USART6 ---- */
    sim_reset();
    k230_send_command(4);
    check("3.1 k230_send_command(4) 发出 \"{4}\"",
          strcmp(sim_tx_capture(), "{4}") == 0, sim_tx_capture());
    sim_reset();
    k230_send_command(9);
    check("3.2 k230_send_command(9) 发出 \"{9}\"",
          strcmp(sim_tx_capture(), "{9}") == 0, sim_tx_capture());
    sim_reset();
    k230_send_command(10);
    check("3.3 越界指令 10 被拒绝(不发送)",
          strcmp(sim_tx_capture(), "") == 0, sim_tx_capture());

    /* ---- 用例4：背靠背连续多帧，验证帧同步不错乱 ---- */
    sim_reset();
    sim_feed_rx("{1}");
    sim_feed_rx("{2}");
    sim_feed_rx("{3}");
    check("4.1 连续三帧后停在最后一帧 {3}",
          strcmp(k230_rxbuf, "{3}") == 0, k230_rxbuf);
    check("4.2 k230_rx_cpl 置 1", k230_rx_cpl == 1, "k230_rx_cpl != 1");

    /* ---- 用例5：帧前有噪声字节，验证 '{' 重新同步 ---- */
    sim_reset();
    sim_feed_rx("xx\xff\x00{7}");
    check("5.1 噪声后仍能同步到 {7}",
          strcmp(k230_rxbuf, "{7}") == 0, k230_rxbuf);
    k230_process();
    check("5.2 解析出 d1=7", k230_d1 == 7, "d1 != 7");

    /* ---- 用例6：未以 '{' 开头的 '}' 不应产生假帧 ---- */
    sim_reset();
    sim_feed_rx("abc}");
    check("6.1 无帧头时 k230_rx_cpl 保持 0", k230_rx_cpl == 0,
          "产生了假帧");

    /* ---- 用例7：溢出错误(ORE)恢复路径 ---- */
    sim_reset();
    sim_feed_rx("{5}");
    sim_uart6_regs.SR |= SIM_USART_SR_ORE; /* 人为制造溢出 */
    USART6_IRQHandler();
    check("7.1 ORE 处理后缓冲被清空", k230_rxbuf[0] == 0, "缓冲未清空");
    check("7.2 ORE 标志被清除", (sim_uart6_regs.SR & SIM_USART_SR_ORE) == 0,
          "ORE 未清除");
    sim_feed_rx("{8}"); /* 溢出后应能继续正常收帧 */
    check("7.3 溢出后仍可正常接收 {8}",
          strcmp(k230_rxbuf, "{8}") == 0, k230_rxbuf);

    /* ---- 用例8：确认 USART6 的 CR1/BRR 未被本次迁移逻辑误写 ---- */
    check("8.1 模拟 USART6 未被写入非法 BRR(=0)",
          sim_uart6_regs.BRR == 0, "BRR 被意外修改");

    printf("---------------------------------------------------------\n");
    printf(" 结果: %d 通过, %d 失败\n", g_pass, g_fail);
    printf("=========================================================\n");
    return g_fail == 0 ? 0 : 1;
}
