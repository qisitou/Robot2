#ifndef __VL53L0_I2C_H
#define __VL53L0_I2C_H

#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK 探索者STM32F407开发板
//VL53L0X IIC驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2017/7/1
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////

//IO方向设置
//#define VL_SDA_IN()  {GPIOA->MODER&=~(3<<(4*2));GPIOA->MODER|=0<<4*2;}	//PB10输入模式
//#define VL_SDA_OUT() {GPIOA->MODER&=~(3<<(4*2));GPIOA->MODER|=1<<4*2;}    //PB10输出模式


//IO操作函数	 
//#define VL_IIC_SCL    PCout(10) 		//SCL
//#define VL_IIC_SDA    PAout(4) 		//SDA	 
//#define VL_READ_SDA   PAin(4) 		    //输入SDA 

#define VL_IIC_PORT		GPIOG
#define VL_IIC_SCL    PGout(4) 		//SCL
#define VL_IIC_SDA    PGout(3) 		//SDA	 
#define VL_READ_SDA   PGin(3) 	    //输入SDA 

//状态
#define STATUS_OK       0x00
#define STATUS_FAIL     0x01

#define   VL53L0X_XshutPort_0   GPIOD
#define   VL53L0X_XshutPin_0    GPIO_Pin_15

#define   VL53L0X_XshutPort_1   GPIOD
#define   VL53L0X_XshutPin_1    GPIO_Pin_0

#define   VL53L0X_XshutPort_2   GPIOD
#define   VL53L0X_XshutPin_2    GPIO_Pin_1

#define   VL53L0X_XshutPort_3   GPIOE
#define   VL53L0X_XshutPin_3    GPIO_Pin_13

#define   VL53L0X_XshutPort_4   GPIOE
#define   VL53L0X_XshutPin_4    GPIO_Pin_14

#define	  VL53L0X_SCL_PIN		GPIO_Pin_4
#define	  VL53L0X_SDA_PIN		GPIO_Pin_3	

//控制Xshut电平,从而使能VL53L0X工作 1:使能 0:关闭
#define VL53L0X_Xshut_0 PDout(15)
#define VL53L0X_Xshut_1 PDout(0)
#define VL53L0X_Xshut_2 PDout(1)
#define VL53L0X_Xshut_3 PEout(13)
#define VL53L0X_Xshut_4 PEout(14)

//IIC操作函数
void VL53L0X_i2c_init(void);//初始化IIC的IO口

u8 VL53L0X_write_byte(u8 address,u8 index,u8 data);              //IIC写一个8位数据
u8 VL53L0X_write_word(u8 address,u8 index,u16 data);             //IIC写一个16位数据
u8 VL53L0X_write_dword(u8 address,u8 index,u32 data);            //IIC写一个32位数据
u8 VL53L0X_write_multi(u8 address, u8 index,u8 *pdata,u16 count);//IIC连续写

u8 VL53L0X_read_byte(u8 address,u8 index,u8 *pdata);             //IIC读一个8位数据
u8 VL53L0X_read_word(u8 address,u8 index,u16 *pdata);            //IIC读一个16位数据
u8 VL53L0X_read_dword(u8 address,u8 index,u32 *pdata);           //IIC读一个32位数据
u8 VL53L0X_read_multi(u8 address,u8 index,u8 *pdata,u16 count);  //IIC连续读


#endif 


