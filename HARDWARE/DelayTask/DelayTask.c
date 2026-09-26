#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "DelayTask.h"
#include "movement.h"

DelayTask *p_head = NULL;
DelayTask *p_tail = NULL;
DelayTask *p_move = NULL;

u16 DelayTask_Num = 0;
u16 DelayTask_Add_Flag = 0;
u32 DelayTask_Add_Arr[20][5];
volatile u32 DelayTask_Tick_10ms = 0;

u16  TIME_S = 0;
u16  TIME_MS = 0;


void DelayTask_Add(u32 Times,u16 Delay_ms, void (*FUNC)(void),char *format, ...)
{
    va_list ap;
    va_start(ap, format);


    if(DelayTask_Add_Flag >= 20)
    {
        va_end(ap);
        return;
    }

    if(NULL != format)
    {
        int *p_param = (int *)malloc(4 * strlen(format)/2);


        if(NULL == p_param)
        {
            va_end(ap);
            return;
        }

        for(int i = 0; i < strlen(format)/2;i++)
        {
            if(('%' == format[i*2] && 'd' == format[i*2+1]) || ('%' == format[i*2] && 's' == format[i*2+1]) || ('%' == format[i*2] && 'c' == format[i*2+1]))
            {
                *(p_param + i) = va_arg(ap,int);
            }
            else if('%' == format[i*2] && 'f' == format[i*2+1])
            {
                *(float*)(p_param + i) = va_arg(ap,double);;
            }
        }

        DelayTask_Add_Arr[DelayTask_Add_Flag][3] = strlen(format)/2;
        DelayTask_Add_Arr[DelayTask_Add_Flag][4] = (u32)p_param;
    }
    else
    {
        DelayTask_Add_Arr[DelayTask_Add_Flag][3] = 0;
        DelayTask_Add_Arr[DelayTask_Add_Flag][4] = (u32)NULL;
    }


    DelayTask_Add_Arr[DelayTask_Add_Flag][0] = Delay_ms;
    DelayTask_Add_Arr[DelayTask_Add_Flag][1] = Times;
    DelayTask_Add_Arr[DelayTask_Add_Flag][2] = (u32)FUNC;


    DelayTask_Add_Flag++;
    va_end(ap);
}



void DelayTask_Times_Add()
{
    for(u16 i = 0; i <DelayTask_Add_Flag; i++)
    {
        DelayTask *p_new = (DelayTask*)malloc(sizeof(DelayTask));

        p_new->Current_DelayTime_ms = 0;
        p_new->DelayTime_ms = DelayTask_Add_Arr[i][0];
        p_new->Times = DelayTask_Add_Arr[i][1];
        p_new->FUNC = (void (*)(void))DelayTask_Add_Arr[i][2];
        p_new->param_num = DelayTask_Add_Arr[i][3];
        p_new->params = (int *)DelayTask_Add_Arr[i][4];
        p_new->next = NULL;

        if(DelayTask_Num > 0)
        {
            p_tail->next = (struct DelayTask *)p_new;
            p_tail = p_new;

        }
        else if(0 == DelayTask_Num)
        {
            p_head = p_new;
            p_tail = p_new;
        }

        DelayTask_Num++;
    }
    DelayTask_Add_Flag = 0;
}




void TIM7_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)
    {
        TIME_MS++;
        if(TIME_MS >= 100)
        {
            TIME_MS = 0;
            TIME_S++;
        }
        DelayTask_Times_Add();  //һ DelayTask_Add() ݴ浽еڵ㡣

        if(p_head != NULL)
        {
            p_move = p_head;
            DelayTask *p_last = NULL;
            while(p_move != NULL)
            {

                if(p_move->Current_DelayTime_ms == p_move->DelayTime_ms)
                {
                    switch(p_move->param_num)
                    {
                        case 0:
                            (*(void(*)())p_move->FUNC)();
                            break;
                        case 1:
                            (*(void(*)())p_move->FUNC)(p_move->params[0]);
                            break;
                        case 2:
                            (*(void(*)())p_move->FUNC)(p_move->params[0],p_move->params[1]);
                            break;
                        case 3:
                            (*(void(*)())p_move->FUNC)(p_move->params[0],p_move->params[1],p_move->params[2]);
                            break;
                        case 4:
                            (*(void(*)())p_move->FUNC)(p_move->params[0],p_move->params[1],p_move->params[2],p_move->params[3]);
                            break;
                        case 5:
                            (*(void(*)())p_move->FUNC)(p_move->params[0],p_move->params[1],p_move->params[2],p_move->params[3],p_move->params[4]);
                            break;
                        case 6:
                            (*(void(*)())p_move->FUNC)(p_move->params[0],p_move->params[1],p_move->params[2],p_move->params[3],p_move->params[4],p_move->params[5]);
                            break;
                        case 7:
                            (*(void(*)())p_move->FUNC)(p_move->params[0],p_move->params[1],p_move->params[2],p_move->params[3],p_move->params[4],p_move->params[5],p_move->params[6]);
                            break;
                        case 8:
                            (*(void(*)())p_move->FUNC)(p_move->params[0],p_move->params[1],p_move->params[2],p_move->params[3],p_move->params[4],p_move->params[5],p_move->params[6],p_move->params[7]);
                            break;
                        case 9:
                            (*(void(*)())p_move->FUNC)(p_move->params[0],p_move->params[1],p_move->params[2],p_move->params[3],p_move->params[4],p_move->params[5],p_move->params[6],p_move->params[7],p_move->params[8]);
                            break;
                        case 10:
                            (*(void(*)())p_move->FUNC)(p_move->params[0],p_move->params[1],p_move->params[2],p_move->params[3],p_move->params[4],p_move->params[5],p_move->params[6],p_move->params[7],p_move->params[8],p_move->params[9]);
                            break;
                    }
                    p_move->Times--;
                    p_move->Current_DelayTime_ms = 0;
                }

                p_move->Current_DelayTime_ms += 10;

                if(0 == p_move->Times)
                {

                    if(p_move != p_head)
                    {
                        p_last->next = p_move->next;
                        if(NULL == p_move->next)
                        {
                            p_tail = p_last;
                        }
                        free(p_move->params);
                        free(p_move);
                        p_move = (DelayTask *)p_last->next;
                    }
                    else{
                        p_head = (DelayTask *)p_move->next;
                        free(p_move->params);
                        free(p_move);
                        p_move = p_head;

                        if(p_move == NULL)
                        {
                            p_tail = NULL;
                        }
                    }

                    DelayTask_Num--;

                }
                else
                {
                    p_last = p_move;
                    p_move = (DelayTask *)p_move->next;
                }
            }
        }

        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    }
}
