#ifndef __CHASSIS_PID_H
#define __CHASSIS_PID_H

#include <stdint.h>
#include "pid.h"

void Chassis_PID_Init(void);	//底盘PID初始化

extern PID_PositionInitTypedef Chassis_AnglePID;
extern PID_PositionInitTypedef Avoid_PID;

#endif
