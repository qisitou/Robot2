/* 最小复现：空循环轮询一个非 volatile 的全局变量（模拟 while(detect_allow==1);） */
typedef unsigned char u8;

u8 detect_allow = 0;

void wait_detect(void)
{
    while (detect_allow == 1);
}
