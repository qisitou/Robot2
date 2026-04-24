#ifndef __STEPMOTOR_H
#define __STEPMOTOR_H

#include <stdint.h>
#include "sys.h"

void StepMotor_Init(void);															//步进初始化
void StepMotor_SetSpeed(uint8_t ID,int32_t Speed);									//步进设置速度
void StepMotor_SetPosition(uint8_t ID,uint16_t Speed,uint32_t Step);				//步进设置位置
void StepMotor_SetPositionExt(uint8_t ID,uint16_t Speed,uint8_t Acc,uint32_t Step);	//步进设置位置(扩展)
void StepMotor_ZDTSyncMove(uint8_t ID);
void StepMotor_ZDTENABLE(uint8_t ID);
void StepMotor_ZDTSetSpeed(uint8_t ID, uint8_t dir, uint16_t vel, uint8_t acc, bool snF);
void StepMotor_ZDTSetPosition(uint8_t ID, uint8_t dir, uint16_t vel, uint8_t acc, uint32_t clk, bool raF, bool snF);
void StepMotor_ZDTStop(uint8_t ID,uint8_t snF);
#endif
