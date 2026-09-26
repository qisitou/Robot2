#include <uart5.h>
#include <usart2.h>
#include "SerialBusServo.h"
#include "string.h"
#include "delay.h"
#include "usart3.h"
#include "usart1.h"
#include "usart6.h"


SerialBusServo Left_Leg =
        {
                "Left_Leg",
                0,
                {0},
                {0},
                0,
                UART5,
                uart5_WriteBuf,
//                USART2,
//                uart2_WriteBuf,
        };

SerialBusServo Right_Leg =
        {
            "Right_Leg",
                0,
                {0},
                {0},
                0,
                USART6,
                uart6_WriteBuf,
        };

//Checksum=~(ID+Length+Cmd+Prm1+...PrmN)若括号内的计算和超出 255, 则取最低的一个字节，“~”表示取反
uint8_t CheckSum(uint8_t arr[])
{
    uint8_t i;
    uint16_t temp = 0;
    for (i = 2; i < arr[3] + 2; i++)
    {
    temp += arr[i];
    }
    temp = ~temp;
    i = (uint8_t)temp;
    return i;
}


void Txbuff_Load(u8 arr[],u8 id,u8 Command,int Param)
{
    arr[0] = arr[1] = LOBOT_SERVO_FRAME_HEADER;  //帧头0x55
    arr[2] = id;                              //id
    arr[4] = Command;               //指令
    if(-1 == Param)
    {
        arr[3] = 3;                                  //数据长度length
        arr[5] = CheckSum(arr);
    }
    else
    {
        arr[3] = 4;                                  //数据长度length
        arr[5] = Param;
        arr[6] = CheckSum(arr);                 //校验和
    }
};


//控制单个舵机转动
void SerialBusServoMove(uint8_t id, int16_t position, uint16_t time)
{
    delay_ms(5);
    printf("id_move:%d\r\n", id);//如果定时器中断里操作串口1，无延时轮询舵机角度和操控它转动，先延时一会，舵机的单片机处理不过来

    SerialBusServo *Leg;

    if(position < 0)
    position = 0;
    if(position > 1000)
    position = 1000;

    if(id <= Single_Leg_Servo_Num)
    {
        Leg = &Left_Leg;
    }
    else
    {
        Leg = &Right_Leg;
    }


    Leg->LobotTxBuf[0] = Leg->LobotTxBuf[1] = LOBOT_SERVO_FRAME_HEADER;//帧头0x55
    Leg->LobotTxBuf[2] = id;                               //id
    Leg->LobotTxBuf[3] = 7;                                //数据长度length
    Leg->LobotTxBuf[4] = LOBOT_SERVO_MOVE_TIME_WRITE;      //指令--写，控制单个舵机转动
    Leg->LobotTxBuf[5] = GET_LOW_BYTE(position);
    Leg->LobotTxBuf[6] = GET_HIGH_BYTE(position);
    Leg->LobotTxBuf[7] = GET_LOW_BYTE(time);
    Leg->LobotTxBuf[8] = GET_HIGH_BYTE(time);
    Leg->LobotTxBuf[9] = CheckSum(Leg->LobotTxBuf);                //校验和

    Leg->uartx_WriteBuf(Leg->LobotTxBuf, 10);
    delay_ms(5);
}


void SerialBusServoMoveWait(uint8_t id, int16_t position, uint16_t time)
{

//    printf("id_move_wait:%d\r\n", id);

    SerialBusServo *Leg;

    if(position < 0)
        position = 0;
    if(position > 1000)
        position = 1000;

    if (id == 7)
    {
        Leg = &Left_Leg;
    }
    else if(id <= Single_Leg_Servo_Num)
    {
        Leg = &Left_Leg;
    }
    else
    {
        Leg = &Right_Leg;
    }


    Leg->LobotTxBuf[0] = Leg->LobotTxBuf[1] = LOBOT_SERVO_FRAME_HEADER;//帧头0x55
    Leg->LobotTxBuf[2] = id;                               //id
    Leg->LobotTxBuf[3] = 7;                                //数据长度length
    Leg->LobotTxBuf[4] = LOBOT_SERVO_MOVE_TIME_WAIT_WRITE;      //指令--写，控制单个舵机转动
    Leg->LobotTxBuf[5] = GET_LOW_BYTE(position);
    Leg->LobotTxBuf[6] = GET_HIGH_BYTE(position);
    Leg->LobotTxBuf[7] = GET_LOW_BYTE(time);
    Leg->LobotTxBuf[8] = GET_HIGH_BYTE(time);
    Leg->LobotTxBuf[9] = CheckSum(Leg->LobotTxBuf);                //校验和

    Leg->uartx_WriteBuf(Leg->LobotTxBuf, 10);

}

void SerialBusServoMoveStart(SerialBusServo *Leg)
{
    Leg->LobotTxBuf[0] = Leg->LobotTxBuf[1] = LOBOT_SERVO_FRAME_HEADER;//帧头0x55
    Leg->LobotTxBuf[2] = 254;                               //id
    Leg->LobotTxBuf[3] = 3;                                //数据长度length
    Leg->LobotTxBuf[4] = LOBOT_SERVO_MOVE_START;      //指令--写，控制单个舵机转动
    Leg->LobotTxBuf[5] = CheckSum(Leg->LobotTxBuf);                //校验和

    Leg->uartx_WriteBuf(Leg->LobotTxBuf, 6);

}

//舵机内部电机是否卸载掉电，0 代表卸载掉电，此时舵机 无力矩输出。1代表装载电机，此时舵机有力矩输出，默认值0
//舵机卸载掉电
void SerialBusServoUnload(SerialBusServo *Leg,uint8_t id)
{
    Txbuff_Load(Leg->LobotTxBuf,id,LOBOT_SERVO_LOAD_OR_UNLOAD_WRITE,0);

    Leg->uartx_WriteBuf(Leg->LobotTxBuf, 7);
    printf("%s-%d-Unload\r\n",Leg->id,id);
    delay_ms(5);
}

//装载电机，有力矩输出
void SerialBusServoLoad(SerialBusServo *Leg,uint8_t id)
{
    Txbuff_Load(Leg->LobotTxBuf,id,LOBOT_SERVO_LOAD_OR_UNLOAD_WRITE,1);

    Leg->uartx_WriteBuf(Leg->LobotTxBuf, 7);
    printf("%s-%d-Load\r\n",Leg->id,id);
//    delay_ms(20);
}


//读取函数
u16 SerailBusServo_READ(SerialBusServo *Leg,u8 id,u8 COMMAND)
{
    u16 ret;
    Leg->rx_Index = 0;
    memset(Leg->LobotRxBuf, 0, sizeof(Leg->LobotRxBuf)); //清空接收缓存区

    Txbuff_Load(Leg->LobotTxBuf,id,COMMAND,-1);

    u16 i;
    sendOnly(Leg->USARTx);
    Leg->uartx_WriteBuf(Leg->LobotTxBuf, 6);
    delay_us(75);   //延时一会，等待最后一个字节发送完成，否则下面的只读指令（关闭发送器）会先完成，导致无法完成发送
    readOnly(Leg->USARTx);

    while(CheckSum(Leg->LobotRxBuf) != Leg->LobotRxBuf[Leg->LobotRxBuf[3]+2]){
        if(i>30000){																											//如果在定时器里操作串口1，这里要加超时退出
            sendOnly(Leg->USARTx);
            printf("overtime\r\n");																							//超时等待，由于可能被定时器中断，导致没有完成接收，卡死在死循环
            return -1;
        }
        i++;
    };

    sendOnly(Leg->USARTx);

    if(7 == Leg->rx_Index)
    {
        ret =  Leg->LobotRxBuf[5];
    }
    else if(8 == Leg->rx_Index)
    {
        ret = (int)BYTE_TO_HW(Leg->LobotRxBuf[6], Leg->LobotRxBuf[5]);
    }
    //接收成功
    return ret;
}


//读取舵机ID
u8 SerialBusServo_Read_ID(SerialBusServo *Leg)
{
    u8 id;
    id = SerailBusServo_READ(Leg,254,LOBOT_SERVO_ID_READ);
	printf("\185\227\178\165ID:254,ID:%d\r\n",id);

    return id;
}

//读取舵机输入电压
int16_t SerialBusServo_Read_VIN(SerialBusServo *Leg,u8 id)
{
    int16_t VIN;
    VIN = SerailBusServo_READ(Leg,id,LOBOT_SERVO_VIN_READ);
    printf("ID:%d,VIN:%d\r\n",id,VIN);

    return VIN;
}

//读取舵机角度
int16_t SerialBusServo_Read_POS(SerialBusServo *Leg,u8 id)
{
    int16_t POS;
    POS = SerailBusServo_READ(Leg,id,LOBOT_SERVO_POS_READ);
    if(POS)
    printf("ID:%d,POS:%d\r\n",id,POS);

    return POS;
}

//读取舵机角度
void USMART_SerialBusServo_Read_POS(u8 id)
{
    int16_t POS;
    if(id < 6)
    {
        POS = SerailBusServo_READ(&Left_Leg,id,LOBOT_SERVO_POS_READ);
    }
    else
    {
        POS = SerailBusServo_READ(&Right_Leg,id,LOBOT_SERVO_POS_READ);
    }
    printf("ID:%d,POS:%d\r\n",id,POS);
}



