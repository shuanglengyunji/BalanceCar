#include "scheduler.h"
#include "attitude_computation.h"
#include "control.h"
#include "movement_control.h"
#include "stdio.h"
#include "time.h"
#include "data_transfer.h"

loop_t loop;

void Duty_1ms(void)
{
	
}

void Duty_2ms(void)
{
	u32 T = 2;
	Attitude_sensor_Read(T);	//读取数据，低通滤波
}


//码盘读取速度数据	
//变量放在全局，方便其他线程调用
float Speed_Left_CM_S = 0;
float Speed_Right_CM_S = 0;
float Desire_Angle = 0;
s16 mydirection = 0;
void Duty_5ms(void)
{
	u32 T = 5;
	s16 Balance_Out_Left,Balance_Out_Right;	//姿态PID输出
//	s16 Speed_Out_Left,Speed_Out_Right;		//速度PID输出
	s16 Out_Left,Out_Right;					//电机控制量
	s16 direction = 0;
	
	
	//PID输出
	Attitude_sensor_Update(T);							//姿态数据更新
	Get_Speed(&Speed_Left_CM_S,&Speed_Right_CM_S,T);	//读取当前速度
			
	//防失控
	if(Angle.y > 15 || Angle.y < -15)
	{
		Out_Left = Out_Right = 0;
	}
	else
	{
		Balance_Control(Angle.y,Gyro.y,&Balance_Out_Left,&Balance_Out_Right,Desire_Angle);			//姿态平衡PID控制
//		Speed_Control(Speed_Left_CM_S,Speed_Right_CM_S,&Speed_Out_Left,&Speed_Out_Right,0.0f);		//速度PID控制
	
		direction = (Speed_Left_CM_S - Speed_Right_CM_S)/6 + mydirection;
	
		//正数代表希望向前转，负数代表希望向后转
		Out_Left  = Balance_Out_Left + direction;  //+ Speed_Out_Left;
		Out_Right = Balance_Out_Right - direction; //+ Speed_Out_Right;
	}
	
	Speed_OutPut(Out_Left,-Out_Right);	//将控制输出数值赋值给电机驱动函数
//	Speed_OutPut(0,0);	//将控制输出数值赋值给电机驱动函数

}

void Duty_10ms(void)
{
	dataupload();
}

void Duty_20ms(void)
{

}

void Duty_50ms(void)
{
	
}


//*************************************************************************************
//运行函数结束


//初始化函数
void Loop_Init(void)
{
	loop.check_flag = 0;
	
	loop.cnt_2ms = 0;
	loop.cnt_5ms = 0;
	loop.cnt_10ms = 0;
	loop.cnt_20ms = 0;
	loop.cnt_50ms = 0;
	
	loop.error_flag = 0;
}

void Duty_Loop(void)
{
	if(loop.check_flag == 1)
	{
		Duty_1ms();							//周期1ms的任务
		
		if(loop.cnt_2ms >= 2)
		{
			loop.cnt_2ms = 0;
			Duty_2ms();						//周期2ms的任务
		}
		if( loop.cnt_5ms >= 5 )
		{
			loop.cnt_5ms = 0;
			Duty_5ms();						//周期5ms的任务
		}
		if( loop.cnt_10ms >= 10 )
		{
			loop.cnt_10ms = 0;
			Duty_10ms();					//周期10ms的任务
		}
		if( loop.cnt_20ms >= 20 )
		{
			loop.cnt_20ms = 0;
			Duty_20ms();					//周期20ms的任务
		}
		if( loop.cnt_50ms >= 50 )
		{
			loop.cnt_50ms = 0;
			Duty_50ms();					//周期50ms的任务
		}
		
		loop.check_flag = 0;	//运行完毕清零
	}
}

void Loop_check(void)  //TIME INTTERRUPT
{
	//这个写法其实并不节省时间，但好在都是整数操作，应该比较快
	//如果时间不够了可以把这部分改掉，有比这个简单得多的写法
	//用单一计数器变量+取余数的方法就行
	loop.cnt_2ms++;
	loop.cnt_5ms++;
	loop.cnt_10ms++;
	loop.cnt_20ms++;
	loop.cnt_50ms++;
	
	if(loop.check_flag == 1)	//如果到这里check_flag到这里还是1，没有被清0，
								//证明主循环里面1ms的任务没有运行完，最后面的check_flag没有运行到
	{
		loop.error_flag++;		//每次出现问题，error_flag+1
	}
	
	loop.check_flag = 1;
}
