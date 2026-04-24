#ifndef __VL53L0X_H
#define __VL53L0X_H

#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"
#include "vl53l0x_gen.h"
#include "vl53l0x_cali.h"
#include "vl53l0x_it.h"
#include "sys.h"
#include "delay.h"

/*====================My_define=====================*/

typedef struct
{
	float vx;			//横向速度								向右为正
	float vy;			//纵向速度								向前为正
	float vw;			//Yaw旋转角度							逆时针为正
	float dis_target;	//目标距离								
	float dis_now;		//当前距离
}Avoid_Data_t; //避障数据结构体

extern Avoid_Data_t avoid_data;//避障数据

/*====================My_define=====================*/


//VL53L0X传感器上电默认IIC地址为0X52(不包含最低位)
#define VL53L0X_Addr 0x52


//使能2.8V IO电平模式
#define USE_I2C_2V8  1

//测量模式
#define Default_Mode   0// 默认
#define HIGH_ACCURACY  1//高精度
#define LONG_RANGE     2//长距离
#define HIGH_SPEED     3//高速

//vl53l0x模式配置参数集
typedef __packed struct
{
	FixPoint1616_t signalLimit;    //Signal极限数值 
	FixPoint1616_t sigmaLimit;     //Sigmal极限数值
	uint32_t timingBudget;         //采样时间周期
	uint8_t preRangeVcselPeriod ;  //VCSEL脉冲周期
	uint8_t finalRangeVcselPeriod ;//VCSEL脉冲周期范围
	
}mode_data;


extern VL53L0X_Dev_t vl53l0x_dev;//设备I2C数据参数
extern VL53L0X_Dev_t vl53l0x_dev0;//设备I2C数据参数
extern VL53L0X_Dev_t vl53l0x_dev1;//设备I2C数据参数
extern VL53L0X_Dev_t vl53l0x_dev2;//设备I2C数据参数
extern VL53L0X_Dev_t vl53l0x_dev3;//设备I2C数据参数
extern VL53L0X_Dev_t vl53l0x_dev4;//设备I2C数据参数
extern VL53L0X_DeviceInfo_t vl53l0x_dev_info;//设备ID版本信息
extern mode_data Mode_data[];
extern uint8_t AjustOK;

void VL53L0X_All_Init(void);

VL53L0X_Error vl53l0x_init(VL53L0X_Dev_t *dev,uint8_t id);//初始化vl53l0x
void print_pal_error(VL53L0X_Error Status);//错误信息打印
void mode_string(u8 mode,char *buf);//模式字符串显示
void vl53l0x_test(void);//vl53l0x测试
void vl53l0x_reset(VL53L0X_Dev_t *dev);//vl53l0x复位

void vl53l0x_info(void);//获取vl53l0x设备ID信息
void One_measurement(u8 mode);//获取一次测量距离数据
#endif


