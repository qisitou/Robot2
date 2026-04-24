#include <stdio.h>
#include "cx522.h"
#include "dma.h"



u8 cx522_allow = 1;  //赋值为1方便单元测试
u8 cx522_rxbuf[50];

/**
 * @brief  初始化CX522读卡器
 * @param  无
 * @retval 无
 * @note   配置UART4的DMA接收模式
 */
void cx522_Init(void)
{
    USART_DMACmd(CX522_UART, USART_DMAReq_Rx, ENABLE);  // 使能UART4的DMA接收请求
    DMA_ITConfig(CX522_DMA_STREAMx, DMA_IT_TC, ENABLE);  // 使能DMA传输完成中断
    MYDMA_Enable(CX522_DMA_STREAMx, 23);  // 启动DMA接收，每次接收23字节
}


/**
 * @brief  处理IC卡数据
 * @param  无
 * @retval 无
 * @note   验证数据帧格式并提取IC卡号码
 */
void cx522_ProcessData(void)
{
    // 验证帧头(0x20)和帧尾(0x03)
    if(0x20 == cx522_rxbuf[0] && 0x03 == cx522_rxbuf[22])
    {
        // 验证校验位(0x00表示成功)
        if(0x00 == cx522_rxbuf[4])
        {
            // 检查是否允许读取
            if(1 == cx522_allow)
            {
                if(0x13 == cx522_rxbuf[10])
                {
                    // GPIO_ResetBits(GPIOD,GPIO_Pin_4);
                }
                
            }
        }
    }
}


/**
 * @brief  DMA1_Stream2中断处理函数
 * @param  无
 * @retval 无
 * @note   处理DMA接收完成中断，处理数据并重新启动DMA
 */
void CX522_IRQHandler(void)
{
    // 检查DMA传输完成中断
    if(DMA_GetITStatus(CX522_DMA_STREAMx, DMA_IT_TCIF2) != RESET)
    {
        DMA_ClearITPendingBit(CX522_DMA_STREAMx, DMA_IT_TCIF2);  // 清除中断标志
        cx522_ProcessData();  // 处理接收到的数据
        MYDMA_Enable(CX522_DMA_STREAMx, 23);  // 重新启动DMA接收
    }
}

