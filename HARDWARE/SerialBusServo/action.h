//
// Created by Seo on 2024/6/19.
//

#ifndef INC_2023GAI_ACTION_H
#define INC_2023GAI_ACTION_H

#include "sys.h"
#include "SerialBusServo.h"

typedef struct
{
    char id[20];
    u16 CurrentAngle[Single_Leg_Servo_Num];
    u16 ResetAngle[Single_Leg_Servo_Num];
    u16 SetAngle[Single_Leg_Servo_Num];

}Leg_Action;

extern Leg_Action Left_Leg_Action;
extern Leg_Action Right_Leg_Action;

extern const u16 Straight_Action_Group[6][12];
extern const u16 Turn_Left_Action_Group[6][12];

void Action_Get_AllAngle(void);

void Action_Get_Left_Leg_Angle(void);

void Action_Get_Right_Leg_Angle(void);

void Action_Reset(void);

void Action_Set_Left_Leg(void);

void Action_Set_Right_Leg(void);

void Action_Reset_Left_Leg(void);

void Action_Reset_Right_Leg(void);

void Action_All_Unload(void);

void Action_All_Load(void);

void Run_Action_Group(const u16 Action_Group[][12], u16 size);

void PC_Run_Action_Group(const u16 Action_Group[][12], u16 size);

void Run_Straight_Action_Group(void);

void Run_Turn_Left_Action_Group(void);

void Run_Turn_Right_Action_Group(void);

void Run_Right_Mid_Action_Group(void);

void Run_Left_Mid_Action_Group(void);

void Run_Right_Lar_Action_Group(void);

void Run_Left_Lar_Action_Group(void);

#endif //INC_2023GAI_ACTION_H
