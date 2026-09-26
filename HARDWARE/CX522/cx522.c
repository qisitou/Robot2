#include <stdio.h>
#include "cx522.h"
#include "dma.h"
#include "uart4.h"
#include "DelayTask.h"
#include "turnplate.h"   //用到 HoleArr 和 Hole_Now_Idx



u8 cx522_allow = 1;  //????1?????????
u8 cx522_rxbuf[23];

// poll read-block-1 command frame
static u8 cx522_read_cmd[] = {0x20,0x00,0x22,0x08,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x01,0xD4,0x03};
                            
// 找一个"完整帧"用的小状态机：0x20 当帧头，0x03 当帧尾（长度不写死，多长都行）
static u8  cx522_frame[23];      // 拼出来的一帧数据（0x20 ... 0x03）
static u16 cx522_frame_len = 0;  // 当前帧已收多少字节
static u8  cx522_frame_on  = 0;  // 1=正在收一帧

// 把收到的每个字节喂进来：0x20 帧头、0x03 帧尾，收齐一帧就交给 cx522_ProcessData() 处理
static void cx522_rx_byte(u8 b)
{
    if(0x20 == b)                             // 帧头：从这里重新开始记一帧
    {
        cx522_frame_len = 0;
        cx522_frame[cx522_frame_len++] = b;
        cx522_frame_on = 1;
    }
    else if(1 == cx522_frame_on)              // 帧中：继续往里记
    {
        if(cx522_frame_len <= sizeof(cx522_frame))
        {
            cx522_frame[cx522_frame_len++] = b;

            if(0x03 == b)                     // 帧尾：一帧收齐了
            {
                cx522_frame_on = 0;
                cx522_ProcessData();
            }
        }
        else                                  // 太长了，肯定不是有效帧，丢掉重新找
        {
            cx522_frame_on = 0;
        }
    }
}

// 把 DMA 刚收到的一串新字节喂给找帧函数（重新开收的动作由调用者做）
static void cx522_rx_process(void)
{
    u16 len = sizeof(cx522_rxbuf) - DMA_GetCurrDataCounter(CX522_DMA_STREAMx);

    for(u16 i = 0; i < len; i++)
    {
        cx522_rx_byte(cx522_rxbuf[i]);
    }
}

// /**
//  * @brief  UART4 中断：总线空闲（IDLE）= 读卡器这一串数据发完了
//  * @param  无
//  * @retval 无
//  */
// void UART4_IRQHandler(void)
// {
//     if(USART_GetITStatus(CX522_UART, USART_IT_IDLE) != RESET)
//     {
//         volatile u32 tmp;
//         tmp = CX522_UART->SR;      // 读 SR 再读 DR 才能清掉 IDLE 标志（顺手把溢出错误也清掉）
//         tmp = CX522_UART->DR;
//         (void)tmp;

//         cx522_rx_process();        // 把这一串收到的数据喂进找帧状态机
//         MYDMA_Enable(CX522_DMA_STREAMx, sizeof(cx522_rxbuf));  // 重新开始收下一串
//     }
// }

/**
 * @brief  ?????CX522??????
 * @param  ??
 * @retval ??
 * @note   ????UART4??DMA??????
 */
void cx522_Init(void)
{
    //USART_ITConfig(CX522_UART, USART_IT_IDLE, ENABLE);  // 打开总线空闲中断：用它发现收了一半的乱数据并重新同步
    USART_DMACmd(CX522_UART, USART_DMAReq_Rx, ENABLE);  // ???UART4??DMA????????
    DMA_ITConfig(CX522_DMA_STREAMx, DMA_IT_TC, ENABLE);  // ???DMA???????????
    MYDMA_Enable(CX522_DMA_STREAMx, sizeof(cx522_rxbuf));  // 收满一整块或总线空闲都会进中断处理

    // 启动 DMA 前清空 UART 接收残留
    volatile u32 tmp;
    tmp = CX522_UART->SR;
    tmp = CX522_UART->DR;
    tmp = CX522_UART->SR;
    (void)tmp;

    DelayTask_Add(100000, 20, cx522_poll, NULL);
}


/**
 * @brief  ????IC??????
 * @param  ??
 * @retval ??
 * @note   ????????????????IC??????
 */
void cx522_ProcessData(void)
{
    // block-read response frame: 20 00 22 11 00 <16 data bytes> sum 03, 23 bytes total
    if((cx522_frame_len >= 23) && (0x22 == cx522_frame[2]) && (0x00 == cx522_frame[1]) && (0x00 == cx522_frame[4]))
    {
        if((1 == cx522_allow) && (0 == HoleArr[Hole_Now_Idx].ic))
        {
            HoleArr[Hole_Now_Idx].ic = cx522_frame[10];
        }
    }
}

// poll task (20ms tick): while cx522_allow==1, send read-block-1 command every 100ms
void cx522_poll(void)
{
    static u8 tick = 0;

    if(1 != cx522_allow)
    {
        return;
    }

    if(tick < 5)
    {
        tick++;
        return;
    }
    tick = 0;

    uart4_WriteBuf((u8 *)cx522_read_cmd, sizeof(cx522_read_cmd));
}


/**
 * @brief  DMA1_Stream2 中断服务函数
 * @param  无
 * @retval 无
 * @note   收满一整块数据会进这个中断，处理完后重新开启 DMA
 */
void DMA1_Stream2_IRQHandler(void)
{
    // 检查 DMA 传输完成中断标志
    if(DMA_GetITStatus(CX522_DMA_STREAMx, DMA_IT_TCIF2) != RESET)
    {
        DMA_ClearITPendingBit(CX522_DMA_STREAMx, DMA_IT_TCIF2);  // 清除中断标志
        cx522_rx_process();  // 处理接收到的数据
        MYDMA_Enable(CX522_DMA_STREAMx, sizeof(cx522_rxbuf));  // 重新开始收下一块
    }
}

