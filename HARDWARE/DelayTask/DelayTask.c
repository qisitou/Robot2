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


/*
Times：执行次数
Delay_ms：延迟毫秒数
FUNC：要执行的函数指针
format：参数格式描述字符串（比如 "%d%d%d%d" 表示 4 个整数）
...：可变参数（实际的参数值）
*/

void DelayTask_Add(u32 Times,u16 Delay_ms, void (*FUNC)(void),char *format, ...)  //还没有 防止同时添加任务 的措施
{
    va_list ap;              //定义一个可变参数遍历变量 ap，用来遍历、读取函数里「不确定个数」的参数(....)
    va_start(ap, format);    // 从 format 后面开始读取参数

    //暂存数组最多 20 条，超出直接丢弃，避免越界写坏内存。
    if(DelayTask_Add_Flag >= 20)
    {
        va_end(ap);
        return;
    }

    if(NULL != format)      //如果 format 不为 NULL，说明有可变参数
    {
        int *p_param = (int *)malloc(4 * strlen(format)/2); //把传进来的可变参数“保存下来”，留到将来延时触发时再用，所以这里用 malloc 在堆里开一块内存（p_param），把每个参数拷贝进去。

        //内存申请失败时直接返回，避免后续空指针写入。
        if(NULL == p_param)
        {
            va_end(ap);
            return;
        }

        for(int i = 0; i < strlen(format)/2;i++)
        {
            if(('%' == format[i*2] && 'd' == format[i*2+1]) || ('%' == format[i*2] && 's' == format[i*2+1]) || ('%' == format[i*2] && 'c' == format[i*2+1]))
            {
                *(p_param + i) = va_arg(ap,int);//从可变参数列表 ap 里取下一个参数，并按 int 类型读取并存到参数数组的第 i 个位置。
            }
            else if('%' == format[i*2] && 'f' == format[i*2+1])
            {
                *(float*)(p_param + i) = va_arg(ap,double);; //核心是这个(float*)，得强转指针类型（编译器问题？）
            }
        }

        DelayTask_Add_Arr[DelayTask_Add_Flag][3] = strlen(format)/2;    // [3] 参数个数：format 里每个参数占2字符（如"%d"），所以/2得到参数个数
        DelayTask_Add_Arr[DelayTask_Add_Flag][4] = (u32)p_param;        // [4] 参数缓存首地址：保存可变参数副本，延时触发时再读取
    }
    else
    {
        DelayTask_Add_Arr[DelayTask_Add_Flag][3] = 0;
        DelayTask_Add_Arr[DelayTask_Add_Flag][4] = (u32)NULL;
    }


    DelayTask_Add_Arr[DelayTask_Add_Flag][0] = Delay_ms;                // [0] 延迟时间
    DelayTask_Add_Arr[DelayTask_Add_Flag][1] = Times;                   // [1] 执行次数
    DelayTask_Add_Arr[DelayTask_Add_Flag][2] = (u32)FUNC;               // [2] 函数指针，强转为u32类型存储，使用时再强转回函数指针类型


    DelayTask_Add_Flag++;
    va_end(ap);            //统一在函数尾部做一次 va_end，避免遗漏清理。
}


//把 DelayTask_Add() 暂存起来的任务，真正生成链表节点，加入到延时任务队列里。
void DelayTask_Times_Add()
{
    for(u16 i = 0; i <DelayTask_Add_Flag; i++)  //DelayTask_Add_Flag 表示当前有多少个任务被暂存在 DelayTask_Add_Arr 里，遍历这些任务，把它们真正加入到链表里。
    {
        DelayTask *p_new = (DelayTask*)malloc(sizeof(DelayTask));   //新建一个链表节点，准备把任务信息装进去

        p_new->Current_DelayTime_ms = 0;                        //当前累计延时清零，表示这个任务刚加入，还没开始计时。
        p_new->DelayTime_ms = DelayTask_Add_Arr[i][0];          //取出这个任务的延迟时间，装到链表节点里
        p_new->Times = DelayTask_Add_Arr[i][1];                 //取出这个任务的执行次数，装到链表节点里
        p_new->FUNC = (void (*)(void))DelayTask_Add_Arr[i][2];  //取出这个任务的函数指针，强转回函数指针类型，装到链表节点里
        p_new->param_num = DelayTask_Add_Arr[i][3];             //取出这个任务的参数个数，装到链表节点里
        p_new->params = (int *)DelayTask_Add_Arr[i][4];         //取出这个任务的参数缓存首地址，强转回 int* 类型，装到链表节点里
        p_new->next = NULL;                                     //新节点的 next 指针清零，表示它暂时还没有下一个节点          

        if(DelayTask_Num > 0)
        {
            p_tail->next = (struct DelayTask *)p_new;   //把当前尾节点的 next 指向新节点
            p_tail = p_new;                             //把新节点更新为尾节点  

        }
        else if(0 == DelayTask_Num) //链表原来是空的时候，也就是刚加入第一个任务节点。
        {
            p_head = p_new; //链表第一个节点
            p_tail = p_new; //同时也是链表最后一个节点
        }

        DelayTask_Num++;
    }
    DelayTask_Add_Flag = 0;
}


//在主循环里调用：把 10ms 节拍处理掉，并执行到期任务。
void DelayTask_Process(void)
{
    u32 tick_count;
    u32 elapsed_ms;

    __disable_irq();
    tick_count = DelayTask_Tick_10ms;
    DelayTask_Tick_10ms = 0;
    __enable_irq();

    if(0 == tick_count)
    {
        return;
    }

    //把“积压的多个 10ms 节拍”一次折算成毫秒，避免 while(tick_count--) 逐拍处理过慢。
    elapsed_ms = tick_count * 10;

    DelayTask_Times_Add();  //把 DelayTask_Add() 里暂存到数组中的任务，真正变成链表节点。

    if(p_head != NULL)      //判断有没有任务，如果链表不空，就开始遍历链表，检查每个任务是否到时间。
    {
        p_move = p_head;    //从链表头开始遍历，p_move 就是当前遍历到的任务节点指针
        DelayTask *p_last = NULL;   //定义一个指针 p_last，专门用来记录 p_move 的上一个节点，方便删除节点时修改链表结构
        while(p_move != NULL)
        {
            u32 total_ms;
            u32 need_run = 0;
            u32 run_count;

            //当前节点累计时间 = 上次剩余时间 + 本次累计到的毫秒数
            total_ms = (u32)p_move->Current_DelayTime_ms + elapsed_ms;

            if(p_move->DelayTime_ms > 0)
            {
                //补偿执行次数：跨过几个周期就补执行几次。
                need_run = total_ms / p_move->DelayTime_ms;
            }
            else
            {
                //Delay=0 视为“立即执行”，本次把剩余次数全部执行完，避免该节点卡住。
                need_run = p_move->Times;
            }

            if(need_run > p_move->Times)
            {
                need_run = p_move->Times;
            }

            run_count = need_run;
            while(run_count--)
            {
                switch(p_move->param_num)   //根据参数个数调用对应函数
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
            }

            p_move->Times -= need_run;

            if(p_move->DelayTime_ms > 0)
            {
                //保留未满一个周期的余数，作为下次累计的起点。
                p_move->Current_DelayTime_ms = total_ms % p_move->DelayTime_ms;
            }

            if(0 == p_move->Times)  //如果这个任务的执行次数已经用完了，就要删除这个任务节点了
            {
                /*  
                这段代码就是“边遍历边安全删除链表节点”。
                删节点时要分“删头”还是“删中间/尾部”
                并且删完后把遍历指针移动到正确位置，避免野指针和断链。
                */
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
}


/*
1.把暂存的任务真正加入链表
2.轮询链表里的任务是否到时间
3.到时间就调用函数
4.执行完就删除任务节点
*/
void TIM7_IRQHandler(void)   //TIM7中断
{
    if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)  //检查TIM更新中断发生与否
    {
				out_time+=10;
				if(out_time >=99999) out_time=0;
        DelayTask_Tick_10ms++;

        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);  //清除TIMx更新中断标志
    }
}
