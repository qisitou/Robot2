#ifndef GRAY_H
#define GRAY_H

#include "sys.h"


#define GRAY_CH1  PFin(3)		//灰度传感器通道1，连接到PF3引脚
#define GRAY_CH2  PFin(2)		//灰度传感器通道2，连接到PF2引脚
#define GRAY_CH3  PFin(1)		//灰度传感器通道3，连接到PF1引脚
#define GRAY_CH4  PFin(0)		//灰度传感器通道4，连接到PF0引脚
#define GRAY_CH5  PBin(4)		//灰度传感器通道5，连接到PB4引脚
#define GRAY_CH6  PBin(9)		//灰度传感器通道6，连接到PB9引脚
#define GRAY_CH7  PEin(0)		//灰度传感器通道7，连接到PE0引脚
#define GRAY_CH8  PEin(1)		//灰度传感器通道8，连接到PE1引脚



void Gray_Init(void);


#endif 
