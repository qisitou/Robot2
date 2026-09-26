#include <delay.h>
#include "usart6.h"
#include "XM1603.h"
#include <stdbool.h>
#include "led.h"

#define GET_LOW_BYTE(A) ((uint8_t)(A))
//宏函数 获得A的低八位
#define GET_HIGH_BYTE(A) ((uint8_t)((A) >> 8))
//宏函数 获得A的高八位


uint8_t XM1603_TxBuf[128];  //发送缓存

u8 QR_allow = 1;
u8 QR_code= 0;


void XM1603_Init()
{
    USART_ITConfig(XM1603_UART, USART_IT_RXNE, DISABLE);//开启相关中断

    XM1603_Write_Boud();
    delay_ms(50);
   XM1603_Write_Byte(0x0000,0x01);//补光灯，瞄准灯拍摄时开 连续模式
   XM1603_Write_Byte(0x0004,0x01);//稳像时长0.1s
   XM1603_Write_Byte(0x0005,0x01);//识读间隔0.1s
   XM1603_Write_Byte(0x0006,0x05);//单次读码时长0.5s
   XM1603_Write_Byte(0x000D,0X00);//数据编码格式为GBK 串口输出
   XM1603_Write_Byte(0x002C,0X01);  //允许识读所有条码，打开旋转功能
   XM1603_Write_Byte(0x003F,0X01);  //允许识读QR码
   XM1603_Write_Byte(0x00B0,0X00);//传送所有Data字符
	//XM1603_Write_Byte(0x0000,0x0E);
    USART_ITConfig(XM1603_UART, USART_IT_RXNE, ENABLE);//开启相关中断
}


void XM1603_Write_Byte(u16 Addr,u8 data)
{
    XM1603_TxBuf[XM1603_TX_HEADER_1] = XM1603_HEADER_1;
    XM1603_TxBuf[XM1603_TX_HEADER_2] = XM1603_HEADER_2;
    XM1603_TxBuf[XM1603_TX_TYPES] = XM1603_TYPES_WRITE;
    XM1603_TxBuf[XM1603_TX_LENS] = 0x01;
    XM1603_TxBuf[XM1603_TX_ADDRESS_HIGH] = Addr >> 8;
    XM1603_TxBuf[XM1603_TX_ADDRESS_LOW] = (u8)Addr;
    XM1603_TxBuf[6] = data;
    XM1603_TxBuf[7] = XM1603_NOCRC_1;
    XM1603_TxBuf[8] = XM1603_NOCRC_2;
    XM1603_WriteBuf(XM1603_TxBuf,9);

    delay_ms(50);
}

void XM1603_Write_Boud()         //9600:0x0139  1115200:0x001A
{
    XM1603_TxBuf[XM1603_TX_HEADER_1] = XM1603_HEADER_1;
    XM1603_TxBuf[XM1603_TX_HEADER_2] = XM1603_HEADER_2;
    XM1603_TxBuf[XM1603_TX_TYPES] = XM1603_TYPES_WRITE;
    XM1603_TxBuf[XM1603_TX_LENS] = 0x02;
    XM1603_TxBuf[XM1603_TX_ADDRESS_HIGH] = 0x00;
    XM1603_TxBuf[XM1603_TX_ADDRESS_LOW] = 0x2A;
    XM1603_TxBuf[6] = 0x1A;
    XM1603_TxBuf[7] = 0x00;
    XM1603_TxBuf[8] = XM1603_NOCRC_1;
    XM1603_TxBuf[9] = XM1603_NOCRC_2;
    XM1603_WriteBuf(XM1603_TxBuf,10);
}



void XM1603_IRQHandler(void)                	//串口1中断服务程序
{
    if(USART_GetITStatus(XM1603_UART, USART_IT_RXNE) != RESET)  //接收中断
    {
        u8 Res = USART_ReceiveData(XM1603_UART);    //(USART1->DR);	//读取接收到的数据

        if(1 == QR_allow)
        {
            if((Res > 0x30) && (Res < 0x34))     // XM1603一开始会发一个 0x02
            {
                QR_code = Res & 0x0F;
                if(QR_code == 1)
                {
                    GPIO_ResetBits(GPIOD,GPIO_Pin_4);
                }
                // QR_allow = 0;
                // printf("qr:%d",QR_code);
            }
        }
    }
}
