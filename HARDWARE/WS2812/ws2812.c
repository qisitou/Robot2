#include <stdio.h>
#include "WS2812.h"
#include "dma.h"



// Some Static Colors
const RGBColor_TypeDef RED             = {255,0,0};
const RGBColor_TypeDef LIGHT_RED      = {25,0,0};
const RGBColor_TypeDef GREEN    = {0,255,0};
const RGBColor_TypeDef BLUE     = {0,0,255};
const RGBColor_TypeDef LIGHT_BLUE     = {0,0,10};
const RGBColor_TypeDef SKY      = {0,255,255};
const RGBColor_TypeDef LIGHT_SKY = {0,10,10};
const RGBColor_TypeDef MAGENTA  = {255,0,255};
const RGBColor_TypeDef LIGHT_MAGENTA = {30,0,30};
const RGBColor_TypeDef YELLOW   = {255,255,0};
const RGBColor_TypeDef ORANGE   = {10,10,0};
const RGBColor_TypeDef BLACK    = {0,0,0};
const RGBColor_TypeDef WHITE    = {255,255,255};
const RGBColor_TypeDef LIGHT_WHITE    = {10,10,10};
const RGBColor_TypeDef PURPLE   = {65,105,225};

u8 pixelBuffer[Pixel_S1_NUM][24];                     //灯珠


/***********************************************************************************************
**     name: WS2812b_Configuration
** function:  WS2812B SPI DMA总线初始化
**parameter: void
************************************************************************************************/
/*****************************************
 说明：
 SPI2：
 引脚：使用的是PB15引脚
 时钟：根据总线图，SPI2由APB1(42MHz)分频而来
 ****************************************/
void WS2812b_Configuration(void){

    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);  //使能SPI2时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;                    //PB15复用功能输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                  //复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;            //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);                        //初始化

    GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);        //PB15复用为 SPI2

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                      //设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		                  //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		                        //串行同步时钟的空闲状态为高电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	                        //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                          //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		//42M/8=5.25M
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                  //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;	                            //CRC值计算的多项式
    SPI_Init(SPI2, &SPI_InitStructure);                                   //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Cmd(SPI2, ENABLE);                                                //使能SPI外设

    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);	        					 // 使能SPI2的DMA发送
    MYDMA_Enable(DMA1_Stream4, Pixel_S1_NUM * 24);              					// 开启DMA传输

    delay_ms(1);
    RGB_BLACK(Pixel_S1_NUM);                                  			 //RGB RESET
    delay_ms(1);
}

/***********************************************************************************************
**     name: rgb_WriteColor
** function: 设定某个RGB LED的颜色
**parameter: void
**   return: void
************************************************************************************************/
void rgb_WriteColor(u16 LedId, RGBColor_TypeDef Color){

    u16 i;

    if( LedId > ( Pixel_S1_NUM ) ){
       // printf("Error:Out of Range!\r\n");
        return;                               //to avoid overflow
    }

    for(i=0;i<=7;i++){
        pixelBuffer[LedId][i]= ( (Color.G & (1 << (7 -i)) )? (CODE1):CODE0 );
    }
    for(i=8;i<=15;i++){
        pixelBuffer[LedId][i]= ( (Color.R & (1 << (15-i)) )? (CODE1):CODE0 );
    }
    for(i=16;i<=23;i++){
        pixelBuffer[LedId][i]= ( (Color.B & (1 << (23-i)) )? (CODE1):CODE0 );
    }
}

/***********************************************************************************************
**     name: rgb_SendArray
** function: Configure colors to RGB pixel series.
             RGBColor_TypeDef: pointer to a RGBColor_TypeDef structure that contains the color configuration information for the RGB pixel.
**parameter: void
**   return: void
************************************************************************************************/
void rgb_SendArray(void){

     if(DMA_GetFlagStatus(DMA1_Stream4,DMA_FLAG_TCIF4) != RESET){ //等待DMA1_Stream4传输完成
        DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);                //清除DMA1_Stream4传输完成标志
        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);	         // 使能SPI2的DMA发送
        MYDMA_Enable(DMA1_Stream4, Pixel_S1_NUM * 24);              // 开启DMA传输
			}
}

/***********************************************************************************************
**     name: rgb_SetColor
** function: 设定某个RGB LED的颜色,并发送
**parameter: void
**   return: void
************************************************************************************************/
void rgb_SetColor(u16 LedId, RGBColor_TypeDef Color){

    rgb_WriteColor(LedId,Color);
    rgb_SendArray();

}


/***********************************************************************************************
**     name: RGB_RED
** function: 设定颜色为RED
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_RED(u16 Pixel_LEN){

    u16 i;

    for(i = 0; i < Pixel_LEN; i++){
        rgb_SetColor(i,RED);
    }

    rgb_SendArray();

}

/***********************************************************************************************
**     name: RGB_PURPLE
** function: 设定颜色为PURPLE
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_PURPLE(u16 Pixel_LEN){

    u16 i;

    for(i = 0; i < Pixel_LEN; i++){
        rgb_SetColor(i,PURPLE);
    }

    rgb_SendArray();
}
/***********************************************************************************************
**     name: RGB_SKY
** function: 设定颜色为SKY
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_SKY(u16 Pixel_LEN){

    u16 i;

    for(i = 0; i < Pixel_LEN; i++){
        rgb_SetColor(i,SKY);
    }

    rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_MAGENTA
** function: 设定颜色为MAGENTA
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_MAGENTA(u16 Pixel_LEN){

    u16 i;

    for(i = 0; i < Pixel_LEN; i++){
        rgb_SetColor(i,MAGENTA);
    }

    rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_ORANGE
** function: 设定颜色为ORANGE
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_ORANGE(u16 Pixel_LEN){

    u16 i;

    for(i = 0; i < Pixel_LEN; i++){
        rgb_SetColor(i,ORANGE);
    }

    rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_GREEN
** function: 设定颜色为GREEN
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_GREEN(u16 Pixel_LEN){

    u16 i;

    for(i = 0; i < Pixel_LEN; i++){
        rgb_SetColor(i,GREEN);
    }

    rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_BLUE
** function: 设定颜色为BLUE
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_BLUE(u16 Pixel_LEN){

    u16 i;

    for(i = 0; i < Pixel_LEN; i++){
        rgb_SetColor(i,BLUE);
    }

    rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_YELLOW
** function: 设定颜色为YELLOW
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_YELLOW(u16 Pixel_LEN){

    u16 i;

    for(i = 0; i < Pixel_LEN; i++){
        rgb_SetColor(i,YELLOW);
    }

    rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_BLACK
** function: 设定颜色为all-off
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_BLACK(u16 Pixel_LEN){

    u16 i;

    for(i = 0; i < Pixel_LEN; i++){

        rgb_SetColor(i,BLACK);
    }

    rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_WHITE
** function: 设定颜色为WHITE
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_WHITE(u16 Pixel_LEN){

    u16 i;

    for(i = 0; i < Pixel_LEN; i++){
        rgb_SetColor(i,WHITE);

    }
    rgb_SendArray();
}

/***********************************************************************************************
**     name: Colourful_Wheel
** function: 将颜色转换为GRB
**parameter: WheelPos 颜色数值
**   return: RGBColor_TypeDef 颜色GRB
************************************************************************************************/
RGBColor_TypeDef Colourful_Wheel(u8 WheelPos){

    RGBColor_TypeDef color;
    WheelPos = 255 - WheelPos;

    if(WheelPos < 85){
        color.R = 255 - WheelPos * 3;
        color.G = 0;
        color.B = WheelPos * 3;
        return color;
    }
    if(WheelPos < 170){
        WheelPos -= 85;
        color.R = 0;
        color.G = WheelPos * 3;
        color.B = 255 - WheelPos * 3;
        return color;
    }

    WheelPos -= 170;
    color.R = WheelPos * 3;
    color.G = 255 - WheelPos * 3;
    color.B = 0;

    return color;
}

/***********************************************************************************************
**     name: rainbowCycle
** function: 呼吸灯功能
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void rainbowCycle(u16 Pixel_LEN){

    u16 i, j = 0;

    for(j = 0; j < 1023; j++){                                                   // 1 cycles of all colors on wheel
        for(i = 0; i < Pixel_LEN; i++){
            rgb_SetColor(i,Colourful_Wheel(((i * 256 / Pixel_LEN) + j)&255));
        }
        rgb_SendArray();
        delay_ms(20);
    }
}

