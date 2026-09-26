#include "headfile.h"

int8_t Hole_Idx = 0;
int8_t Hole_Now_Idx = 0;

int8_t last_Idx = 0;
extern u16 wait_time = 0;

int idx_bias = 0;

/*====== 检测状态机相关变量 ======*/
u8 shake_flag = 0;        //摇转盘标志（shake函数用）

u8 detect_allow = 0;      //检测总开关：1=允许检测
u8 detect_stage = 2;      //检测阶段：当前只有阶段2（找目标球）
u8 detect_flag = 1;       //阶段内步骤：0=等待延时，1/2/3
u8 detect_cpl = 1;        //检测完成标志：1=已完成
u8 continue_flag = 0;     //同步信号：延时2秒后置1

int8_t turnplate_dir = 1; //转盘扫描方向：1=正向，-1=反向

char target_color = 'r';  //我方目标色
char anti_color = 'b';    //敌方色

u16 timeout_S = 0;        //超时计时起点（秒）

int8_t hole =0;


struct Hole HoleArr[10] = {
        {.ball=1, .pos = 2480},
        {.ball=1, .pos = 2270},
        {.ball=1, .pos = 2070},
        {.ball=1, .pos = 1870},
        {.ball=1, .pos = 1670},
        {.ball=1, .pos = 1460},
        {.ball=1, .pos = 1260},
        {.ball=1, .pos = 1060},
        {.ball=1, .pos = 860},
        {.ball=1, .pos = 660},
};

uint8_t Ball_falling(void)  //返回1表示没有球落下，返回0表示有球落下
{
    static uint8_t gpio_initialized = 0;

    if (gpio_initialized == 0)
    {
        GPIO_InitTypeDef GPIO_InitStructure;

        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_Init(GPIOF, &GPIO_InitStructure);

        gpio_initialized = 1;
    }

    return GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_13);
}

void Turnplate_SetPos(u16 pos)
{
    TIM_SetCompare1(TIM3,pos);
}

void Turnplate_Move(int8_t idx)
{
    idx %= 10;
    if(idx < 0) idx += 10;

    cx522_allow = 0;
    Hole_Now_Idx = idx;
    idx_bias = idx - last_Idx;

    if(idx_bias < 0)
    {
        wait_time = -idx_bias*260;
    }
    else
    {
        wait_time = idx_bias*260;
    }
    if(idx_bias>5||idx_bias<-5)
    {
        wait_time=wait_time+1000;
    }
    last_Idx = idx;

    Turnplate_SetPos(HoleArr[idx].pos);

    DelayTask_Add(1,wait_time+200,(void (*)(void)) change_flag,"%d%d",&cx522_allow,1);
}

void turnplate_blank(void)
{
    Hole_Idx = 0;
    while(HoleArr[Hole_Idx].ball == 0)
    {
        Hole_Idx++;
    }

    Turnplate_Move(Hole_Idx-3);
}

void turnplate_daoduo(u8 row)
{
    int8_t res = 0;

    for(int8_t i = 0; i < 10; i++)
    {
        if((row<<4|row) == HoleArr[i].ball)
        {
            res = i;
        }
    }

    if (res < 3)
    {
        Hole_Idx = res + 7;
    } else
    {
        Hole_Idx = res - 3;
    }
    Turnplate_Move(Hole_Idx);
}

void shake()
{
    if(1 == shake_flag)
    {
        Turnplate_SetPos(HoleArr[Hole_Idx].pos-40);
        DelayTask_Add(1,300,(void (*)(void)) change_flag,"%d%d",&shake_flag,2);
        shake_flag = 0;
    }
    else if(2 == shake_flag)
    {
        Turnplate_SetPos(HoleArr[Hole_Idx].pos+40);
        DelayTask_Add(1,300,(void (*)(void)) change_flag,"%d%d",&shake_flag,1);
        shake_flag = 0;
    }
}


void turnplate_log(void)
{
    // printf("\r\n<<<\r\n");
    for (int i = 0; i < 10; i++)
    {
        // printf("id:%d,ball:%c,ic:%#x\r\n", i, HoleArr[i].ball, HoleArr[i].ic);
    }
    // printf(">>>\r\n");
}


// void turnplate_detect()
// {
//     if(1 == detect_allow) {
//         if (2 == detect_stage)
//         {
//             if (1 == detect_flag)
//             {
//                 if(1 == turnplate_dir)
//                 {
//                     Hole_Idx = 0;
//                 }
//                 else if(-1 == turnplate_dir)
//                 {
//                     Hole_Idx = 9;
//                 }

//                 detect_flag = 2;
//                 detect_cpl = 0;

//                 DelayTask_Add(1, 2000, (void (*)(void)) change_flag, "%d%d", &continue_flag, 1);
//             }
//             else if (2 == detect_flag)
//             {
//                 while ((target_color != HoleArr[Hole_Idx].ball) ||
//                        ((0 != HoleArr[Hole_Idx].ic) && (target_color == HoleArr[Hole_Idx].ball))) {
//                     Hole_Idx += turnplate_dir;
// //                    printf("Hole_Idx:%d\r\n", Hole_Idx);
//                     if (((-1 == Hole_Idx) && (-1 == turnplate_dir)) || ((TURNPLATE_HOLE_NUM == Hole_Idx) && (1 == turnplate_dir)))
//                     {
//                         for (int i = 0; i < 10; i++)
//                         {
//                             printf("id:%d,ball:%c,ic:%#x\r\n", i, HoleArr[i].ball, HoleArr[i].ic);
//                         }

//                         turnplate_dir = 1;

//                         detect_cpl = 1;
//                         detect_flag = 1;
//                         detect_allow = 0;
//                         return;
//                     }
//                 }

//                 Hole_Idx_tmp = Hole_Idx;

//                 if (Hole_Idx < 9)
//                 {
//                     Hole_Idx = Hole_Idx + 1;
//                 } else {
//                     Hole_Idx = 0;
//                 }

//                 printf("Hole_Idx:%#x,Hole_Idx_tmp:%#x\n", Hole_Idx, Hole_Idx_tmp);
//                 Turnplate_Move(Hole_Idx);
//                 shake_flag = 1;
//                 detect_flag = 0;
//                 timeout_S = TIME_S;
//                 DelayTask_Add(1, wait_time+50, (void (*)(void)) change_flag, "%d%d", &detect_flag, 3);
//             }
//             else if (3 == detect_flag)
//             {
// //                if(TIME_S > (timeout_S + 20))
// //                {
// //                    printf("detect_timeout\r\n");
// //                    HoleArr[Hole_Idx_tmp].ball = 0;
// //                    HoleArr[Hole_Idx_tmp].ic = 0;
// //                    detect_flag = 2;
// //                    return;
// //                }
//                 if(TIME_S > (timeout_S + 20))
//                 {
//                     printf("detect_timeout\r\n");
//                     detect_flag = 1;

//                     return;
//                 }

//                 if (0 == HoleArr[Hole_Idx_tmp].ic)
//                 {
//                     shake();
//                 } else {
//                     detect_flag = 2;
//                 }
//             }
//         }

//     }
// }

void turnplate_detect_test()
{
    if(1 == detect_allow)
    {
        if (1 == detect_flag)
        {
            Hole_Idx = 0;
            detect_flag = 2;
            detect_cpl = 0;
        }
        else if (2 == detect_flag)
        {
            while ( HoleArr[Hole_Idx].ball==1 ||  HoleArr[Hole_Idx].ic !=0) 
            {
                Hole_Idx += 1;
//                    printf("Hole_Idx:%d\r\n", Hole_Idx);
                if (Hole_Idx==10)
                {
                    for(int i=0;i<10;i++)
                    {
                        printf("id:%d,ball:%d,ic:%#x\r\n",i,HoleArr[i].ball,HoleArr[i].ic);
                    }
                    detect_cpl = 1;
                    detect_flag = 1;
                    detect_allow = 0;
                    return;

                }
            }
            Turnplate_Move(Hole_Idx);
            shake_flag = 1;
            detect_flag = 0;
            timeout_S = TIME_S;
            DelayTask_Add(1, wait_time+300, (void (*)(void)) change_flag, "%d%d", &detect_flag, 3);
        }
        else if (3 == detect_flag)
        {
            if(TIME_S > (timeout_S + 6))
            {
                printf("detect_timeout\r\n");
                HoleArr[Hole_Idx].ic = 1;
                detect_flag = 2;
                return;
            }

            if (0 == HoleArr[Hole_Idx].ic)
            {
                shake();
            } else {
                detect_flag = 2;
            }
        }
    }
}




// void turnplate_detect_test()
// {
//     if(1 == detect_allow) {
//         if (2 == detect_stage)
//         {
//             if (1 == detect_flag)
//             {
//                 if(1 == turnplate_dir)
//                 {
//                     Hole_Idx = 0;
//                 }
//                 else if(-1 == turnplate_dir)
//                 {
//                     Hole_Idx = 9;
//                 }

//                 detect_flag = 2;
//                 detect_cpl = 0;
//             }
//             else if (2 == detect_flag)
//             {
//                 while ( HoleArr[Hole_Idx].ball==1 ||  HoleArr[Hole_Idx].ic !=0) {
//                     Hole_Idx += turnplate_dir;
// //                    printf("Hole_Idx:%d\r\n", Hole_Idx);
//                     if (((-1 == Hole_Idx) && (-1 == turnplate_dir)) || ((TURNPLATE_HOLE_NUM == Hole_Idx) && (1 == turnplate_dir)))
//                     {
//                         for(int i=0;i<10;i++)
//                         {
//                             printf("id:%d,ball:%d,ic:%#x\r\n",i,HoleArr[i].ball,HoleArr[i].ic);
//                         }
//                         turnplate_dir = 1;

//                         detect_cpl = 1;
//                         detect_flag = 1;
//                         detect_allow = 0;
//                         return;

//                     }
//                 }
//                 Turnplate_Move(Hole_Idx);
//                 shake_flag = 1;
//                 detect_flag = 0;
//                 timeout_S = TIME_S;
//                 DelayTask_Add(1, wait_time+50, (void (*)(void)) change_flag, "%d%d", &detect_flag, 3);
//             }
//             else if (3 == detect_flag)
//             {
// //                if(TIME_S > (timeout_S + 20))
// //                {
// //                    printf("detect_timeout\r\n");
// //                    HoleArr[Hole_Idx_tmp].ball = 0;
// //                    HoleArr[Hole_Idx_tmp].ic = 0;
// //                    detect_flag = 2;
// //                    return;
// //                }
//                 if(TIME_S > (timeout_S + 20))
//                 {
//                    HoleArr[Hole_Idx].ic = 1;
//                    detect_flag = 2;
//                    return;
//                 }

//                 if (0 == HoleArr[Hole_Idx].ic)
//                 {
//                     shake();
//                 } else {
//                     detect_flag = 2;
//                 }
//             }
//         }

//     }
// }