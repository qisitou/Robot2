#ifndef __WS2812_H
#define __WS2812_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "delay.h"

#define RGB_1 3
#define RGB_2 2
#define RGB_3 1
#define RGB_4 0

typedef struct
{
    u8 R;
    u8 G;
    u8 B;
} RGBColor_TypeDef;

#define Pixel_S1_NUM 4		//灯珠 RGB数量
extern u8 pixelBuffer[Pixel_S1_NUM][24];
extern const RGBColor_TypeDef RED ;
extern const RGBColor_TypeDef LIGHT_RED ;
extern const RGBColor_TypeDef GREEN;
extern const RGBColor_TypeDef BLUE;
extern const RGBColor_TypeDef LIGHT_BLUE;
extern const RGBColor_TypeDef SKY;
extern const RGBColor_TypeDef LIGHT_SKY;
extern const RGBColor_TypeDef MAGENTA ;
extern const RGBColor_TypeDef LIGHT_MAGENTA;
extern const RGBColor_TypeDef YELLOW ;
extern const RGBColor_TypeDef ORANGE;
extern const RGBColor_TypeDef BLACK;
extern const RGBColor_TypeDef WHITE;
extern const RGBColor_TypeDef LIGHT_WHITE;
extern const RGBColor_TypeDef PURPLE;
/* Exported constants --------------------------------------------------------*/


/**************************************
 说明：

 WS2812B编码协议(单位：ns)：
       min     typ     max
bit 0
 T0H:  220      -      420
 T0L:  750      -      1600

bit 1
 T1H:  750      -      1600
 T1L:  220      -      420


 RESET: time > 300us


8分频APB1，42MHz/8 = 5.25MHz
时钟周期为：1/5.25/1e6 = 1.90e-7=190ns
**************************************/

#define CODE0 0xC0 // 0码, 发送的时间 1100 0000  根据不同的SCK适当调整
#define CODE1 0xFC // 1码, 发送的时间 1111 1100

void WS2812b_Configuration(void);

void rgb_WriteColor(u16 LedId, RGBColor_TypeDef Color);
void rgb_SendArray(void);
void rgb_SetColor(u16 LedId, RGBColor_TypeDef Color);

/* Basic Color Effects -------------------------------------------------------*/
void RGB_RED(u16 Pixel_LEN);
void RGB_GREEN(u16 Pixel_LEN);
void RGB_BLUE(u16 Pixel_LEN);
void RGB_YELLOW(u16 Pixel_LEN);
void RGB_MAGENTA(u16 Pixel_LEN);
void RGB_BLACK(u16 Pixel_LEN);
void RGB_WHITE(u16 Pixel_LEN);
void RGB_SKY(u16 Pixel_LEN);
void RGB_ORANGE(u16 Pixel_LEN);
void RGB_PURPLE(u16 Pixel_LEN);
/* Complicated Color Effects -------------------------------------------------*/
void rainbowCycle(u16 Pixel_LEN);

#endif /* __WS2812_H */

