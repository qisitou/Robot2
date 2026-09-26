/*
 * stm32f4xx.h —— .simtest 宿主机仿真用的桩头文件
 * k230.c 链路上会间接包含它，这里只需转发到 sys.h 的模拟定义。
 */
#ifndef __SIM_STM32F4XX_H
#define __SIM_STM32F4XX_H
#include "sys.h"
#define STM32F40_41xxx 1
#endif
