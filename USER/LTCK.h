#ifndef __LTCK_H
#define __LTCK_H

#include "sys.h"
typedef enum
{
  None=0,	//未选择
  Red,    //红方
  Blue,   //蓝方
}Color;// 

void LTCK_Init(void);
void Choose_Color(void);
void Go_To_Turntable(void);
void Go_To_Stairs(void);
void Go_To_Warehouse(void);
void Go_To_Home(void);



#endif

