//
// Created by Seo on 2024/6/19.
//

#include <stdio.h>
#include "action.h"
#include "delay.h"

#define USE_WAIT 1

const u16 Straight_Action_Group[6][12] = {
        {1,450,530,370,685,515,520,505,265,680,455,585},
        {2,450,570,475,580,470,455,470,540,395,450,525},
        {3,450,520,605,405,490,510,515,595,375,485,555},
        {4,450,490,695,335,435,475,570,555,440,545,530},
        {5,450,455,530,535,435,485,605,500,470,490,550},
        {6,450,435,400,645,415,500,605,460,530,515,530},
};
//const u16 Straight_Action_Group[6][12] = {
//        {1,350,530,370,685,515,520,505,265,680,455,585},
//        {2,350,570,475,580,470,455,470,540,395,450,525},
//        {3,350,520,605,405,490,510,515,595,375,485,555},
//        {4,350,490,695,335,435,475,570,555,440,545,530},
//        {5,350,455,530,535,435,485,605,500,470,490,550},
//        {6,350,435,400,645,415,500,605,460,530,515,530},
//};


//350
//const u16 Straight_Action_Group[6][12] = {
//        {1,400,530,370,685,515,520,505,265,680,455,585},
//        {2,400,570,475,580,470,455,470,540,395,450,525},
//        {3,400,520,605,405,490,510,515,595,375,485,555},
//        {4,400,490,695,365,435,475,570,555,440,545,530},
//        {5,400,430,530,555,435,485,605,500,460,490,550},
//        {6,400,435,400,645,415,500,605,460,530,515,530},
//};//350


//const u16 Turn_Left_Action_Group[6][12] = {
//        {1,450,530,370,670,505,465,505,275,680,475,585},
//        {2,450,570,475,580,490,455,470,540,395,450,525},
//        {3,450,520,605,405,490,510,515,610,375,485,530},
//        {4,450,490,695,340,435,475,570,555,430,545,530},
//        {5,450,430,495,595,435,445,605,500,460,510,550},
//        {6,450,470,350,675,460,425,605,450,550,520,530},
//};
//const u16 Turn_Left_Action_Group[6][12] = {
//        {1,350,530,370,670,505,465,505,275,680,475,585},
//        {2,350,570,475,580,490,455,470,540,395,450,525},
//        {3,350,520,605,405,490,510,515,610,375,485,530},
//        {4,350,490,695,340,435,475,570,555,430,545,530},
//        {5,350,430,495,595,435,445,605,500,460,510,550},
//        {6,350,470,350,675,460,425,605,450,550,520,530},
//};
const u16 Turn_Left_Action_Group[6][12] = {
        {1,450,530,370,670,505,465,505,275,680,475,585},
        {2,450,570,475,580,490,455,470,540,395,450,525},
        {3,450,520,605,405,490,510,515,610,375,485,530},
        {4,450,490,695,340,435,475,570,555,430,545,530},
        {5,450,450,510,575,445,445,605,500,480,495,550},
        {6,450,470,365,700,445,425,605,450,550,520,530},
};


const u16 Turn_Right_Action_Group[7][12] = {
        {1,500,475,365,635,490,525,600,360,670,510,485},
        {2,500,525,355,690,485,515,550,370,565,520,495},
        {3,500,515,345,725,490,470,505,435,495,515,615},
        {4,500,555,435,625,490,430,530,545,400,510,595},
        {5,500,520,640,370,510,495,530,685,340,510,635},
        {6,500,470,550,575,430,530,600,560,340,565,630},
        {7,500,480,400,665,460,500,600,505,455,520,550}
};

const u16 Left_Mid_Action_Group[3][12] = {
        {1,500,480,400,665,460,515,600,505,455,495,550},
        {2,500,475,520,530,510,350,550,530,440,510,585},
        {3,500,525,470,545,480,495,520,505,500,520,525},
};

const u16 Right_Mid_Action_Group[3][12] = {
        {1,500,480,400,665,460,515,600,505,455,500,550},
        {2,500,470,365,675,430,555,585,545,440,515,450},
        {3,500,500,500,510,500,525,530,420,575,535,575},
};

const u16 Left_Ltl_Action_Group[3][12] = {
        {1,400,480,400,660,435,515,595,530,445,485,550},
        {2,400,475,520,530,510,410,550,530,440,510,540},
        {3,400,525,470,545,480,495,520,505,500,520,525},
};

const u16 Right_Ltl_Action_Group[4][12] = {
        {1,500,480,400,665,465,455,590,505,485,500,550},
        {2,500,445,430,620,435,510,600,545,465,475,475},
        {3,500,485,400,675,430,510,530,545,440,515,480},
        {4,500,500,530,495,500,465,540,420,580,495,555},
};

const u16 Left_Lar_Action_Group[3][12] = {
        {1,450,465,390,650,455,515,595,530,445,510,550},
        {2,450,465,390,685,475,415,595,510,410,530,615},
        {3,450,525,435,615,480,495,520,505,440,520,525},
};


const u16 Right_Lar_Action_Group[4][12] = {
        {1,450,480,400,665,465,455,590,505,455,510,550},
        {2,450,445,400,670,405,530,600,545,440,515,495},
        {3,450,485,400,675,430,530,530,545,440,515,495},
        {4,450,500,530,495,500,465,540,420,560,495,585},
};



Leg_Action Left_Leg_Action =
        {
                "Left_Leg_Action",
                {0},
                {499,494,532,491,492},
                {699,336,918,469,594}
        };

Leg_Action Right_Leg_Action =
        {
                "Right_Leg_Action",
                {0},
                {550,499,495,495,550},
                {551,524,509,482,550}
        };

//void Action_Get_AllAngle()
//{
//    for (u8 i = 0; i <Single_Leg_Servo_Num; i++)
//    {
//        SerialBusServo_Read_POS(&Left_Leg,u8 id);
//    }
//}

void Action_Get_Left_Leg_Angle(void)
{
    for (u8 i = 0; i <Single_Leg_Servo_Num; i++)
    {
        Left_Leg_Action.CurrentAngle[i] = SerialBusServo_Read_POS(&Left_Leg,i+1);
//        delay_ms(2);
    }
}

void Action_Get_Right_Leg_Angle(void)
{
    for (u8 i = 0; i <Single_Leg_Servo_Num; i++)
    {
        Right_Leg_Action.CurrentAngle[i] = SerialBusServo_Read_POS(&Right_Leg,i+Single_Leg_Servo_Num+1);
//        delay_ms(2);
    }
}


void Action_Reset_Left_Leg()
{
    for (u8 i = 0; i <Single_Leg_Servo_Num; i++)
    {
#if USE_WAIT==1
        SerialBusServoMoveWait(i+1,Left_Leg_Action.ResetAngle[i],400);
//        delay_ms(5);
#else
        SerialBusServoMove(i+1,Left_Leg_Action.ResetAngle[i],500);
#endif
    }
}

void Action_Reset_Right_Leg()
{
    for (u8 i = 0; i <Single_Leg_Servo_Num; i++)
    {

#if USE_WAIT==1
        SerialBusServoMoveWait(i+Single_Leg_Servo_Num+1,Right_Leg_Action.ResetAngle[i],400);
//        delay_ms(5);
#else
        SerialBusServoMove(i+Single_Leg_Servo_Num+1,Right_Leg_Action.ResetAngle[i],500);
#endif
    }
}

void Action_Reset()
{
    printf("Action_Reset\r\n");
#if USE_WAIT==1
    Action_Reset_Left_Leg();
    Action_Reset_Right_Leg();
    SerialBusServoMoveStart(&Left_Leg);
//    delay_ms(5);
    SerialBusServoMoveStart(&Right_Leg);

#else
    Action_Reset_Left_Leg();
    Action_Reset_Right_Leg();
#endif
    printf("Action_Reset_Complete\r\n");
}

void Action_Set_Left_Leg()
{
    for (u8 i = 0; i <Single_Leg_Servo_Num; i++)
    {
        SerialBusServoMove(i+1,Left_Leg_Action.SetAngle[i],50);
    }
}

void Action_Set_Right_Leg()
{
    for (u8 i = 0; i <Single_Leg_Servo_Num; i++)
    {
        SerialBusServoMove(i+Single_Leg_Servo_Num+1,Right_Leg_Action.SetAngle[i],50);
    }
}

void Action_All_Unload()
{

    for (u8 i = 0; i <Single_Leg_Servo_Num; i++)
    {
        SerialBusServoUnload(&Left_Leg,i+1);
        SerialBusServoUnload(&Right_Leg,i+Single_Leg_Servo_Num+1);
    }

}

void Action_All_Load()
{
    for (u8 i = 0; i <Single_Leg_Servo_Num; i++)
    {
        SerialBusServoLoad(&Left_Leg,i+1);
        SerialBusServoLoad(&Right_Leg,i+Single_Leg_Servo_Num+1);
    }

}

void Run_Action_Group(const u16 Action_Group[][12], u16 size)
{
//    printf("Size:%d\r\n",size);
    for (u8 i = 0; i < size; i++)
    {
//        printf("time:%d\r\n", Action_Group[i][1]);
        for (u8 j = 0; j <5; j++)
        {
//            SerialBusServoMove(j+1,Action_Group[i][j+2],Action_Group[i][1]);
            SerialBusServoMoveWait(j+1,Action_Group[i][j+2],Action_Group[i][1]);
            delay_ms(2);
//            SerialBusServoMoveWait(j+1,Action_Group[i][j+2],Action_Group[i][1]);
//            delay_ms(8);
//            SerialBusServoMoveWait(j+Single_Leg_Servo_Num+1,Action_Group[i][j+Single_Leg_Servo_Num+2],Action_Group[i][1]);
//            delay_ms(5);
//            printf("pos:%d\r\n", Action_Group[i][j+2]);
//            printf("pos:%d\r\n", Action_Group[i][j+Single_Leg_Servo_Num+2]);
        }

        for (u8 j = 0; j <5; j++)
        {
//            SerialBusServoMove(j+Single_Leg_Servo_Num+1,Action_Group[i][j+Single_Leg_Servo_Num+2],Action_Group[i][1]);
            SerialBusServoMoveWait(j+Single_Leg_Servo_Num+1,Action_Group[i][j+Single_Leg_Servo_Num+2],Action_Group[i][1]);
            delay_ms(2);
//            SerialBusServoMoveWait(j+Single_Leg_Servo_Num+1,Action_Group[i][j+Single_Leg_Servo_Num+2],Action_Group[i][1]);
//            delay_ms(8);
        }
        SerialBusServoMoveStart(&Left_Leg);
        delay_ms(2);
        SerialBusServoMoveStart(&Right_Leg);
//        delay_ms(8);
//        printf("time:%d\r\n", Action_Group[i][1]+200);
//        delay_ms(Action_Group[i][1]+200);
//        delay_ms(Action_Group[i][1]+50);
//        delay_ms(Action_Group[i][1]+15);
        delay_ms(Action_Group[i][1]-10);
//        SerialBusServoMove(i+Single_Leg_Servo_Num+1,Right_Leg_Action.SetAngle[i],50);
    }
//    for (u8 i = 0; i <Single_Leg_Servo_Num; i++)
//    {
//        SerialBusServoMove(i+Single_Leg_Servo_Num+1,Right_Leg_Action.SetAngle[i],50);
//    }
//    Action_Reset();
}

void PC_Run_Action_Group(const u16 Action_Group[][12], u16 size)
{
    printf("Size:%d\r\n",size);
    for (u8 i = 0; i < size; i++)
    {
        printf("time:%d\r\n", Action_Group[i][1]);
        for (u8 j = 0; j <5; j++)
        {
            SerialBusServoMoveWait(j+1,Action_Group[i][j+2],Action_Group[i][1]);
            delay_ms(8);
//            SerialBusServoMoveWait(j+1,Action_Group[i][j+2],Action_Group[i][1]);
//            delay_ms(8);
        }

        for (u8 j = 0; j <5; j++)
        {
            SerialBusServoMoveWait(j+Single_Leg_Servo_Num+1,Action_Group[i][j+Single_Leg_Servo_Num+2],Action_Group[i][1]);
            delay_ms(8);
//            SerialBusServoMoveWait(j+Single_Leg_Servo_Num+1,Action_Group[i][j+Single_Leg_Servo_Num+2],Action_Group[i][1]);
//            delay_ms(8);
        }
        SerialBusServoMoveStart(&Left_Leg);
        delay_ms(8);
        SerialBusServoMoveStart(&Right_Leg);
        delay_ms(8);
    }
}


void Run_Straight_Action_Group(void)
{
    Run_Action_Group(Straight_Action_Group,sizeof(Straight_Action_Group) / sizeof(Straight_Action_Group[0]));
//    Action_Reset();
}

void Run_Turn_Left_Action_Group(void)
{
    Run_Action_Group(Turn_Left_Action_Group,sizeof(Turn_Left_Action_Group) / sizeof(Turn_Left_Action_Group[0]));
//    Action_Reset();
}

void Run_Turn_Right_Action_Group(void)
{
    Run_Action_Group(Turn_Right_Action_Group,sizeof(Turn_Right_Action_Group) / sizeof(Turn_Right_Action_Group[0]));
    Action_Reset();
}

void Run_Right_Mid_Action_Group(void)
{
    Run_Action_Group(Right_Mid_Action_Group,sizeof(Right_Mid_Action_Group) / sizeof(Right_Mid_Action_Group[0]));
    Action_Reset();
}

void Run_Left_Mid_Action_Group(void)
{
    Run_Action_Group(Left_Mid_Action_Group,sizeof(Left_Mid_Action_Group) / sizeof(Left_Mid_Action_Group[0]));
    Action_Reset();
}

void Run_Right_Ltl_Action_Group(void)
{
    Run_Action_Group(Right_Ltl_Action_Group,sizeof(Right_Ltl_Action_Group) / sizeof(Right_Ltl_Action_Group[0]));
//    Action_Reset();
}

void Run_Left_Ltl_Action_Group(void)
{
    Run_Action_Group(Left_Ltl_Action_Group,sizeof(Left_Ltl_Action_Group) / sizeof(Left_Ltl_Action_Group[0]));
//    Action_Reset();
}
void Run_Right_Lar_Action_Group(void)
{
    Run_Action_Group(Right_Lar_Action_Group,sizeof(Right_Lar_Action_Group) / sizeof(Right_Lar_Action_Group[0]));
//    Action_Reset();
}

void Run_Left_Lar_Action_Group(void)
{
    Run_Action_Group(Left_Lar_Action_Group,sizeof(Left_Lar_Action_Group) / sizeof(Left_Lar_Action_Group[0]));
//    Action_Reset();
}
