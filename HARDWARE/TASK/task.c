#include "headfile.h"

u8 SubTask_flag = 1;

void change_SubTask(u8 flag)
{
    SubTask_flag = flag;
}


void Run(void)
{
    switch (SubTask_flag)
    {
    case 0:
        {
						Stop_now();
						//SubTask_flag=2;
					
        }
        break;
    case 1:
        { 
            out_time = 0;while(out_time  <  500) 	{Move_Mode2(40,40,0,0,0);}
						out_time = 0;while(out_time  <  2000) {Move_Mode2(0,40,0,0,0);}
						out_time = 0;while(out_time  <  1000) {Move_Mode2(0,0,0,0,0);}
            //Move_Mode1(30,0,0,0);
            //DelayTask_Add(1, 2000,(void (*)(void)) change_SubTask, "%d", 2);
            SubTask_flag=0;     
        }
        break;

    case 2:
        {
 

        }
        break;
    default:
        break;
    }


}
