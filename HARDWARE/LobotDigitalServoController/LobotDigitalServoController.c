/*******************************************************************************
* 文件名： LobotServoController.c
* 作者： 深圳乐幻索尔科技
* 日期：20160806
* LSC系列舵机控制板二次开发示例
*******************************************************************************/
#include "LobotDigitalServoController.h"
#include "dma.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <timer.h>

#define GET_LOW_BYTE(A) ((uint8_t)(A))
//宏函数 获得A的低八位
#define GET_HIGH_BYTE(A) ((uint8_t)((A) >> 8))
//宏函数 获得A的高八位

bool isUartRxCompleted;
bool isActionGroupCompleted = true;

uint8_t LobotTxBuf[128];  //发送缓存
uint8_t LobotRxBuf[16];
uint8_t LobotRxIdx;
uint8_t LobotRxDataLen;
uint16_t batteryVolt;

u16 last_pos = 2450;

u16 action_timeout_S = 0;

u16 kv = 1;

/*********************************************************************************
 * Function:  moveServo
 * Description： 控制单个舵机转动
 * Parameters:   sevoID:舵机ID，Position:目标位置,Time:转动时间
                    舵机ID取值:0<=舵机ID<=31,Time取值: Time > 0
 * Return:       无返回
 * Others:
 **********************************************************************************/
void moveServo(uint8_t servoID, uint16_t Position, uint16_t Time)
{
    if (servoID > 31 || !(Time > 0)) {  //舵机ID不能打于31,可根据对应控制板修改
        return;
    }

    int pos_bias = Position - last_pos;

    kv = 1;
    if(pos_bias < -800 || pos_bias > 800)
    {
        Time = 500;
        kv = 2;
    }

    if(pos_bias < -1400 || pos_bias > 1400)
    {
        Time = 1000;
        kv = 2;
    }
    //printf("time:%d",Time);

    last_pos = Position;

    LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;    //填充帧头
    LobotTxBuf[2] = 8;
    LobotTxBuf[3] = CMD_SERVO_MOVE;           //数据长度=要控制舵机数*3+5，此处=1*3+5//填充舵机移动指令
    LobotTxBuf[4] = 1;                        //要控制的舵机个数
    LobotTxBuf[5] = GET_LOW_BYTE(Time);       //取得时间的低八位
    LobotTxBuf[6] = GET_HIGH_BYTE(Time);      //取得时间的高八位
    LobotTxBuf[7] = servoID;                  //舵机ID
    LobotTxBuf[8] = GET_LOW_BYTE(Position);   //取得目标位置的低八位
    LobotTxBuf[9] = GET_HIGH_BYTE(Position);  //取得目标位置的高八位


    if(DMA_GetFlagStatus(DMA1_Stream3,DMA_FLAG_TCIF3)!=RESET)//等待DMA1_Steam3传输完成
    {
        DMA_ClearFlag(DMA1_Stream3,DMA_FLAG_TCIF3);//清除DMA1_Steam3传输完成标志
    }

    USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);  //使能串口3的DMA发送
    MYDMA_Enable(DMA1_Stream3,10);     //开始一次DMA传输！

}

/*********************************************************************************
 * Function:  moveServosByArray
 * Description： 控制多个舵机转动
 * Parameters:   servos[]:舵机结体数组，Num:舵机个数,Time:转动时间
                    0 < Num <= 32,Time > 0
 * Return:       无返回
 * Others:
 **********************************************************************************/
void moveServosByArray(LobotServo servos[], uint8_t Num, uint16_t Time)
{
    uint8_t index = 7;
    uint8_t i = 0;

    if (Num < 1 || Num > 32 || !(Time > 0)) {
        return;                                          //舵机数不能为零和大与32，时间不能为零
    }
    LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;      //填充帧头
    LobotTxBuf[2] = Num * 3 + 5;                       //数据长度 = 要控制舵机数*3+5
    LobotTxBuf[3] = CMD_SERVO_MOVE;                    //填充舵机移动指令
    LobotTxBuf[4] = Num;                               //要控制的舵机个数
    LobotTxBuf[5] = GET_LOW_BYTE(Time);                //取得时间的低八位
    LobotTxBuf[6] = GET_HIGH_BYTE(Time);               //取得时间的高八位

    for (i = 0; i < Num; i++) {                        //循环填充舵机ID和对应目标位置
        LobotTxBuf[index++] = servos[i].ID;              //填充舵机ID
        LobotTxBuf[index++] = GET_LOW_BYTE(servos[i].Position); //填充目标位置低八位
        LobotTxBuf[index++] = GET_HIGH_BYTE(servos[i].Position);//填充目标位置高八位
    }

    uart3_WriteBuf(LobotTxBuf, LobotTxBuf[2] + 2);             //发送
}

/*********************************************************************************
 * Function:  moveServos
 * Description： 控制多个舵机转动
 * Parameters:   Num:舵机个数,Time:转动时间,...:舵机ID,转动角，舵机ID,转动角度 如此类推
 * Return:       无返回
 * Others:
 * eg: moveServos(2, 800, 2,1200,9,2300); //800毫秒2号舵机到1200位置，9号舵机到2300位置
 **********************************************************************************/
void moveServos(uint8_t Num, uint16_t Time, ...)
{
    uint8_t index = 7;
    uint8_t i = 0;
    uint16_t temp;
    va_list arg_ptr;  //

    va_start(arg_ptr, Time); //取得可变参数首地址
    if (Num < 1 || Num > 32) {
        return;               //舵机数不能为零和大与32，时间不能小于0
    }
    LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;      //填充帧头
    LobotTxBuf[2] = Num * 3 + 5;                //数据长度 = 要控制舵机数 * 3 + 5
    LobotTxBuf[3] = CMD_SERVO_MOVE;             //舵机移动指令
    LobotTxBuf[4] = Num;                        //要控制舵机数
    LobotTxBuf[5] = GET_LOW_BYTE(Time);         //取得时间的低八位
    LobotTxBuf[6] = GET_HIGH_BYTE(Time);        //取得时间的高八位

    for (i = 0; i < Num; i++) {//从可变参数中取得并循环填充舵机ID和对应目标位置
        temp = va_arg(arg_ptr, int);//可参数中取得舵机ID
        LobotTxBuf[index++] = GET_LOW_BYTE(((uint16_t)temp));
        temp = va_arg(arg_ptr, int);  //可变参数中取得对应目标位置
        LobotTxBuf[index++] = GET_LOW_BYTE(((uint16_t)temp)); //填充目标位置低八位
        LobotTxBuf[index++] = GET_HIGH_BYTE(temp);//填充目标位置高八位
    }

    va_end(arg_ptr);  //置空arg_ptr

    uart3_WriteBuf(LobotTxBuf, LobotTxBuf[2] + 2);    //发送
}


/*********************************************************************************
 * Function:  runActionGroup
 * Description： 运行指定动作组
 * Parameters:   NumOfAction:动作组序号, Times:执行次数
 * Return:       无返回
 * Others:       Times = 0 时无限循环
 **********************************************************************************/
void runActionGroup(uint8_t numOfAction, uint16_t Times, bool Wait)
{

    if(0 != numOfAction)
    {
        isActionGroupCompleted = false;
    }

    memset(LobotRxBuf, 0, sizeof(LobotRxBuf)); //清空接收缓存区

    LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;  //填充帧头
    LobotTxBuf[2] = 5;                      //数据长度，数据帧除帧头部分数据字节数，此命令固定为5
    LobotTxBuf[3] = CMD_ACTION_GROUP_RUN;   //填充运行动作组命令
    LobotTxBuf[4] = numOfAction;            //填充要运行的动作组号
    LobotTxBuf[5] = GET_LOW_BYTE(Times);    //取得要运行次数的低八位
    LobotTxBuf[6] = GET_HIGH_BYTE(Times);   //取得要运行次数的高八位


    if(DMA_GetFlagStatus(DMA1_Stream3,DMA_FLAG_TCIF3)!=RESET)//等待DMA1_Steam3传输完成
    {
        DMA_ClearFlag(DMA1_Stream3,DMA_FLAG_TCIF3);//清除DMA1_Steam3传输完成标志
    }

    USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE);  //使能串口3的DMA发送
    MYDMA_Enable(DMA1_Stream3,7);     //开始一次DMA传输！

//    if(Wait) //true：等待动作组运行结束  false 不等
//    {
//        u32 wait_time = TIME_mS;
//        printf("SEND\r\n");
//        while( !(LobotRxBuf[3]== 0x08  &&  LobotRxBuf[4] == numOfAction  &&  (LobotRxBuf[5]+(LobotRxBuf[6]<<8)) == Times) )
//        {
//            if(TIME_mS > (wait_time+3000)) //等3s都没有完成动作就再发一次
//            {
//                printf("Again SEND\r\n");
//                uartWriteBuf(LobotTxBuf, 7); //串口发送
//                break;
//            }
//
//        }
//        printf("runActionGroupCompleted\r\n");
//    }

}

/*********************************************************************************
 * Function:  stopActiongGroup
 * Description： 停止动作组运行
 * Parameters:   Speed: 目标速度
 * Return:       无返回
 * Others:
 **********************************************************************************/
void stopActionGroup(void)
{
    LobotTxBuf[0] = FRAME_HEADER;     //填充帧头
    LobotTxBuf[1] = FRAME_HEADER;
    LobotTxBuf[2] = 2;                //数据长度，数据帧除帧头部分数据字节数，此命令固定为2
    LobotTxBuf[3] = CMD_ACTION_GROUP_STOP;   //填充停止运行动作组命令

    uart3_WriteBuf(LobotTxBuf, 4);      //发送
}
/*********************************************************************************
 * Function:  setActionGroupSpeed
 * Description： 设定指定动作组的运行速度
 * Parameters:   NumOfAction: 动作组序号 , Speed:目标速度
 * Return:       无返回
 * Others:
 **********************************************************************************/
void setActionGroupSpeed(uint8_t numOfAction, uint16_t Speed)
{
    LobotTxBuf[0] = LobotTxBuf[1] = FRAME_HEADER;   //填充帧头
    LobotTxBuf[2] = 5;                       //数据长度，数据帧除帧头部分数据字节数，此命令固定为5
    LobotTxBuf[3] = CMD_ACTION_GROUP_SPEED;  //填充设置动作组速度命令
    LobotTxBuf[4] = numOfAction;             //填充要设置的动作组号
    LobotTxBuf[5] = GET_LOW_BYTE(Speed);     //获得目标速度的低八位
    LobotTxBuf[6] = GET_HIGH_BYTE(Speed);    //获得目标熟读的高八位

    uart3_WriteBuf(LobotTxBuf, 7);             //发送
}

/*********************************************************************************
 * Function:  setAllActionGroupSpeed
 * Description： 设置所有动作组的运行速度
 * Parameters:   Speed: 目标速度
 * Return:       无返回
 * Others:
 **********************************************************************************/
void setAllActionGroupSpeed(uint16_t Speed)
{
    setActionGroupSpeed(0xFF, Speed);  //调用动作组速度设定，组号为0xFF时设置所有组的速度
}

/*********************************************************************************
 * Function:  getBatteryVoltage
 * Description： 发送获取电池电压命令
 * Parameters:   Timeout：重试次数
 * Return:       无返回
 * Others:
 **********************************************************************************/
void getBatteryVoltage(void)
{
    LobotTxBuf[0] = FRAME_HEADER;  //填充帧头
    LobotTxBuf[1] = FRAME_HEADER;
    LobotTxBuf[2] = 2;             //数据长度，数据帧除帧头部分数据字节数，此命令固定为2
    LobotTxBuf[3] = CMD_GET_BATTERY_VOLTAGE;  //填充获取电池电压命令

    uart3_WriteBuf(LobotTxBuf, 4);   //发送
}

void USART3_IRQHandler(void)                	//串口3中断服务程序
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
    {
        u8 Res = USART_ReceiveData(USART3);	//读取接收到的数据
        if(0 == LobotRxIdx)                 //帧头：连续收到两个0x55，表示有数据包到达
        {
            if(FRAME_HEADER == Res)
            {
                LobotRxBuf[LobotRxIdx] = Res;
                LobotRxIdx++;
            }
        }
        else if(1 == LobotRxIdx)
        {
            // 第二个字节也必须是帧头，才认为帧起始有效
            if(FRAME_HEADER == Res)
            {
                LobotRxBuf[LobotRxIdx] = Res;
                LobotRxIdx++;
            }
            else
            {
                LobotRxIdx = 0;
                LobotRxDataLen = 0;
            }
        }
        else if(2 == LobotRxIdx)
        {
            LobotRxBuf[LobotRxIdx] = Res;
            LobotRxIdx++;

            if(LobotRxDataLen == 0 )
            {
                if(3 == LobotRxIdx)
                {
                    LobotRxDataLen = Res;
                }
            }
            else
            {
                if(LobotRxIdx == (LobotRxDataLen + 2)) //数据长度：参数个数N 加上一个指令 加上数据长度本身占用的一个字节长度，即数据长度：Length = N + 2
                {

                    //isUartRxCompleted = true;
                    if(CMD_ACTION_GROUP_COMPLETE == LobotRxBuf[3])
                    {
                        isActionGroupCompleted = true;
                        //printf("Completed\r\n");
                    }
                    LobotRxIdx = 0;
                    LobotRxDataLen = 0;
                }
            }

        }
    }
}


