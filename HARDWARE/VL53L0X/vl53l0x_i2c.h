#ifndef __VL53L0_I2C_H
#define __VL53L0_I2C_H

#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////









//All rights reserved
//////////////////////////////////////////////////////////////////////////////////







//#define VL_IIC_SCL    PCout(10) 		//SCL
//#define VL_IIC_SDA    PAout(4) 		//SDA


#define VL_IIC_PORT		GPIOG
#define VL_IIC_SCL    PGout(4) 		//SCL
#define VL_IIC_SDA    PGout(3) 		//SDA
#define VL_READ_SDA   PGin(3)

//״̬
#define STATUS_OK       0x00
#define STATUS_FAIL     0x01

#define   VL53L0X_XshutPort_0   GPIOD
#define   VL53L0X_XshutPin_0    GPIO_Pin_15

#define   VL53L0X_XshutPort_1   GPIOD
#define   VL53L0X_XshutPin_1    GPIO_Pin_15

#define   VL53L0X_XshutPort_2   GPIOD
#define   VL53L0X_XshutPin_2    GPIO_Pin_0

#define   VL53L0X_XshutPort_3   GPIOD
#define   VL53L0X_XshutPin_3    GPIO_Pin_1

#define   VL53L0X_XshutPort_4   GPIOE
#define   VL53L0X_XshutPin_4    GPIO_Pin_13

#define   VL53L0X_XshutPort_5   GPIOE
#define   VL53L0X_XshutPin_5    GPIO_Pin_14

#define	  VL53L0X_SCL_PIN		GPIO_Pin_4
#define	  VL53L0X_SDA_PIN		GPIO_Pin_3


#define VL53L0X_Xshut_0 PDout(15)
#define VL53L0X_Xshut_1 PDout(15)
#define VL53L0X_Xshut_2 PDout(0)
#define VL53L0X_Xshut_3 PDout(1)
#define VL53L0X_Xshut_4 PEout(13)
#define VL53L0X_Xshut_5 PEout(14)

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


