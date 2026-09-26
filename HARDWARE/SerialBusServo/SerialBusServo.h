#ifndef LOBOTSERVOCONTROLLER_H_
#define LOBOTSERVOCONTROLLER_H_

#include "sys.h"

#define LOBOT_SERVO_FRAME_HEADER         0x55
#define LOBOT_SERVO_MOVE_TIME_WRITE      1
#define LOBOT_SERVO_MOVE_TIME_READ       2
#define LOBOT_SERVO_MOVE_TIME_WAIT_WRITE 7
#define LOBOT_SERVO_MOVE_TIME_WAIT_READ  8
#define LOBOT_SERVO_MOVE_START           11
#define LOBOT_SERVO_MOVE_STOP            12
#define LOBOT_SERVO_ID_WRITE             13
#define LOBOT_SERVO_ID_READ              14
#define LOBOT_SERVO_ANGLE_OFFSET_ADJUST  17
#define LOBOT_SERVO_ANGLE_OFFSET_WRITE   18
#define LOBOT_SERVO_ANGLE_OFFSET_READ    19
#define LOBOT_SERVO_ANGLE_LIMIT_WRITE    20
#define LOBOT_SERVO_ANGLE_LIMIT_READ     21
#define LOBOT_SERVO_VIN_LIMIT_WRITE      22
#define LOBOT_SERVO_VIN_LIMIT_READ       23
#define LOBOT_SERVO_TEMP_MAX_LIMIT_WRITE 24
#define LOBOT_SERVO_TEMP_MAX_LIMIT_READ  25
#define LOBOT_SERVO_TEMP_READ            26
#define LOBOT_SERVO_VIN_READ             27
#define LOBOT_SERVO_POS_READ             28
#define LOBOT_SERVO_OR_MOTOR_MODE_WRITE  29
#define LOBOT_SERVO_OR_MOTOR_MODE_READ   30
#define LOBOT_SERVO_LOAD_OR_UNLOAD_WRITE 31
#define LOBOT_SERVO_LOAD_OR_UNLOAD_READ  32
#define LOBOT_SERVO_LED_CTRL_WRITE       33
#define LOBOT_SERVO_LED_CTRL_READ        34
#define LOBOT_SERVO_LED_ERROR_WRITE      35
#define LOBOT_SERVO_LED_ERROR_READ       36


#define readOnly(x)	x->CR1 |= 4;	x->CR1 &= 0xFFFFFFF7;		//串口x配置为只读，CR1->RE=1, CR1->TE=0
#define sendOnly(x)	x->CR1 |= 8;	x->CR1 &= 0xFFFFFFFB;		//串口x配置为只写，CR1->RE=0, CR1->TE=1


#define GET_LOW_BYTE(A) ((uint8_t)(A))        //宏函数 获得A的低八位
#define GET_HIGH_BYTE(A) ((uint8_t)((A) >> 8))         //宏函数 获得A的高八位
#define BYTE_TO_HW(H, L) ((((uint16_t)(H)) << 8) | (uint8_t)(L))   //宏函数 将高低八位合成为十六位

#define min_VIN  5000     //最低电压5000毫伏
#define max_VIN  12500
#define min_pos  0
#define max_pos  1000     //最大角度1000

#define USART1_Leg Left_Leg
#define USART6_Leg Right_Leg

#define Single_Leg_Servo_Num 5

typedef struct
{
    char id[10];
    u8 Res;
    u8 LobotTxBuf[128];
    u8 LobotRxBuf[16];
    u8 rx_Index;
    USART_TypeDef* USARTx;
    void (*uartx_WriteBuf)(uint8_t *buf, uint8_t len);
}SerialBusServo;

extern SerialBusServo Left_Leg;

extern SerialBusServo Right_Leg;

uint8_t CheckSum(uint8_t buf[]);    //校验和
void SerialBusServoSetID(SerialBusServo *Leg,uint8_t oldID, uint8_t newID);//ID设置
void SerialBusServoMove(uint8_t id, int16_t position, uint16_t time);//控制单个舵机转动角度
void SerialBusServoMoveWait(uint8_t id, int16_t position, uint16_t time);//控制单个舵机转动角度
void SerialBusServoMoveStart(SerialBusServo *Leg);
void SerialBusServoUnload(SerialBusServo *Leg,uint8_t id);  //卸电舵机
void SerialBusServoLoad(SerialBusServo *Leg,uint8_t id);    //装载舵机
void SerialBusServo_ANGLE_OFFSET(uint8_t id, signed char pc);//设置偏差，不保存
void SerialBusServo_ANGLE_OFFSET_WRITE(uint8_t id);  //保存偏差
void SerialBusServo_VIN_WRITE(uint8_t id);  //电压范围设置
void SerialBusServo_ANGLE_WRITE(uint8_t id);//角度范围设置


void SerialBusServo_Read_OFFSET(uint8_t id);    //读取偏差
int16_t SerialBusServo_Read_VIN(SerialBusServo *Leg,u8 id);   //读取输入电压
int16_t SerialBusServo_Read_POS(SerialBusServo *Leg,u8 id);   //读取舵机角度


u16 SerailBusServo_READ(SerialBusServo *Leg,u8 id,u8 COMMAND);
u8 SerialBusServo_Read_ID(SerialBusServo *Leg);       //广播获取ID


void USMART_SerialBusServo_Read_POS(u8 id);


#endif

