#include "movement_control.h"
#include "bsp_encoder.h"
#include "bsp_motor.h"
#include "stdlib.h"
#include "include.h"

//输出向方向控制IO和PWM输出
void Speed_OutPut(s32 Speed_Left,s32 Speed_Right)
{
	Left_Motor(Speed_Left);
	Right_Motor(Speed_Right);
}

/*
 * 停止:IN1 0 IN2 0
 * 正转:IN1 1 IN2 0
 * 反转:IN1 0 IN2 1
 *
 * 车轮与编码器参数
 * 轮直径：65mm
 * 编码器线数：390
 * 车轮转一圈，输出390个脉冲
*/
void Get_Speed(float * Speed_Left_CM_S,float * Speed_Right_CM_S,u32 T)
{
	s32 Lnumber,Rnumber;
	
	Lnumber = Get_Left_Speed();		//读取编码器
	Rnumber = -Get_Right_Speed();	//读取编码器
	
	*Speed_Left_CM_S  = (float)Lnumber / (float)T * 52.3598775f;		// (6.5*3.14159265 / 390) / ( T / 1000 ) = 52.3598775
	*Speed_Right_CM_S = (float)Rnumber / (float)T * 52.3598775f;
}

