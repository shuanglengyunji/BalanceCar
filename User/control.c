#include "control.h"
#include "include.h"

/*
 * 说明
 * control.c是针对平衡小车制作的试验性姿态控制代码文件
 * 本文件的调用方式为：
 * 1.调用其中函数
 * 2.用extern中的变量访问数据输出
 * 3.控制所需入参在函数调用时传入
 * 
 * 本文件中的函数没有严格的使用指针传递数据，也没有对所有轴的控制都加以处理。
 * 所有的代码都是试验性的，稍显混乱，在正式版代码中会更正。
 * 
*/

float kpb,kib,kdb;
float kps,kis,kds;

/*
 * 姿态控制用变量初始化
 * 初始化内容：pid中的 kp kd
 */
void Control_Init(void)
{
	//姿态PID
	kpb = 66;
	kib = 3;
	kdb = 3;
	
	//速度PID
	kps = 3.5;
	kis = 0;
	kds = 1;
}

/*
 * 为方便运算，在运算过程中可以使用float，但运算后对电机的控制量输出应为 s16类型，取值范围+1000 ~ -1000
 *
 * 传入参数 Angle_y 					float	y轴倾角			-180° ~ +180°
 *					Gyro_y						float	y轴角速度		-2000°/s ~ +2000°/s
 * 					Expect_Angle_y 		float	y轴期望倾角		-180° ~  +180°
 * 传出参数 Control_Out_Left	s16		左轮速度控制量  +1000 ~ -1000
 *          Control_Out_Right	s16		右轮速度控制量	+1000 ~ -1000
 *
 */
float error_sum = 0;
void Balance_Control(float Angle_y,float Gyro_y,s16 *Control_Out_Left,s16 *Control_Out_Right,float Expect_Angle_y)
{
	float error;
	
	s16 pid_out;
	
	error = Expect_Angle_y - Angle_y;	//前倾 -- 正，后仰 -- 负
	error_sum += error;
	error_sum = LIMIT(error_sum,-32767,32767);	//限幅
	pid_out = kpb * error + error_sum * kib- kdb * Gyro_y;	//PID控制，由于角速度是角度的一阶导数，所以满足pid公式中d项要求，不用除dt，直接乘dt就行了。
														//如果前倒，Gyro是负数，但是需要向后仰方向调整，所以前面是负号
	
//	pid_out = 0;
	
	//希望后仰调整（车辆过度前倾） -- 输出正数		希望前倾调整（车辆过度后仰） -- 输出负数
	*Control_Out_Left = pid_out;
	*Control_Out_Right = pid_out;
}

/*
 * 速度控制函数
 * 传入参数	Speed_Left				s16		左轮速度
 * 					Speed_Right				s16		右轮速度
 *					Expect_Speed			s16		期望速度
 * 传出参数	Control_Out_Left	s16 *	左轮速度控制量	+1000 ~ -1000
 *					Control_Out_Right	s16 *	右轮速度控制量	+1000 ~ -1000
 *
*/
void Speed_Control(s16 Speed_Left,s16 Speed_Right,s16 *Control_Out_Left,s16 *Control_Out_Right,float Expect_Speed)
{
	float Lerror,Rerror;
	s16 LOut,ROut;
	
	Lerror = Speed_Left  - Expect_Speed;	//向前转，Speed_Left为负数，error为负数
	Rerror = Speed_Right - Expect_Speed;	//向前转，Speed_Right为负数，error为负数
	
	LOut = (s16)(kps * Lerror);
	ROut = (s16)(kps * Rerror);
	
	*Control_Out_Left  = LOut;
	*Control_Out_Right = ROut;
}

//******************************************************************************************************************
//The End


///**************************************************************************
//函数功能：直立PD控制
//入口参数：角度、角速度
//返回  值：直立控制PWM
//作    者：平衡小车之家
//**************************************************************************/
//int balance(float Angle,float Gyro)
//{  
//   float Bias,kp=300,kd=1;
//	 int balance;
//	
//	//这个是代表传感器没安正，有点歪？
//	 Bias=Angle-ZHONGZHI;       //===求出平衡的角度中值 和机械相关
//	
//	//这行PID控制还真是简单啊，看来难度都在姿态解算和速度控制上了
//	 balance=kp*Bias+Gyro*kd;   //===计算平衡控制的电机PWM  PD控制   kp是P系数 kd是D系数 
//	
//	 return balance;
//}

///**************************************************************************
//函数功能：速度PI控制 修改前进后退速度，请修Target_Velocity，比如，改成60就比较慢了
//入口参数：左轮编码器、右轮编码器
//返回  值：速度控制PWM
//作    者：平衡小车之家
//**************************************************************************/
//int velocity(int encoder_left,int encoder_right)
//{  
//	static float Velocity,Encoder_Least,Encoder,Movement;
//	static float Encoder_Integral,Target_Velocity;
//	float kp=80,ki=0.4;
//	
//	//=============遥控前进后退部分=======================// 
//	/*
//	1.控制前后（正负）
//	*/

//	if(1==Flag_Qian)    	//前进
//		Movement=Target_Velocity/Flag_sudu;	         //===前进标志位置1 
//	else if(1==Flag_Hou)	//后退
//		Movement=-Target_Velocity/Flag_sudu;         //===后退标志位置1
//	else  Movement=0;		//停

//	//=============速度PI控制器=======================//	
//	Encoder_Least =(Encoder_Left+Encoder_Right)-0;                    //==获取最新速度偏差==
//																														// = 测量速度（左右编码器之和）- 目标速度（此处为零） 

//	//对速度偏差进行低通滤波，这个滤波的滤波程度还挺大的啊！
//	//我觉得这个叫速度偏差不大正确，应该叫做平均前进速度，以为左右编码器取的是和
//	Encoder *= 0.8;		                                                //===一阶低通滤波器       
//	Encoder += Encoder_Least*0.2;	                                    //===一阶低通滤波器    

//	//记得有一句话，说速度控制是正反馈。跑得快时想慢一点，就要跑的再快点。
//	Encoder_Integral +=Encoder;                                       //===积分出位移 积分时间：10ms
//	Encoder_Integral=Encoder_Integral-Movement;                       //===接收遥控器数据，控制前进后退
//	if(Encoder_Integral>10000)  	Encoder_Integral=10000;             //===积分限幅
//	if(Encoder_Integral<-10000)	Encoder_Integral=-10000;              //===积分限幅	
//	Velocity=Encoder*kp+Encoder_Integral*ki;                          //===速度控制	
//	if(Turn_Off(Angle_Balance,Voltage)==1||Flag_Stop==1)   Encoder_Integral=0;      //===电机关闭后清除积分
//	return Velocity;
//}

///**************************************************************************
//函数功能：转向控制  修改转向速度，请修改Turn_Amplitude即可
//入口参数：左轮编码器、右轮编码器、Z轴陀螺仪
//返回  值：转向控制PWM
//作    者：平衡小车之家
//**************************************************************************/
//int turn(int encoder_left,int encoder_right,float gyro)//转向控制
//{
//	 static float Turn_Target,Turn,Encoder_temp,Turn_Convert=0.9,Turn_Count;
//	  float Turn_Amplitude=88/Flag_sudu,Kp=42,Kd=0;     
//	  //=============遥控左右旋转部分=======================//
//  	if(1==Flag_Left||1==Flag_Right)                      //这一部分主要是根据旋转前的速度调整速度的起始速度，增加小车的适应性
//		{
//			if(++Turn_Count==1)
//			Encoder_temp=myabs(encoder_left+encoder_right);
//			Turn_Convert=50/Encoder_temp;
//			if(Turn_Convert<0.6)Turn_Convert=0.6;
//			if(Turn_Convert>3)Turn_Convert=3;
//		}	
//	  else
//		{
//			Turn_Convert=0.9;
//			Turn_Count=0;
//			Encoder_temp=0;
//		}			
//		if(1==Flag_Left)	           Turn_Target-=Turn_Convert;
//		else if(1==Flag_Right)	     Turn_Target+=Turn_Convert; 
//		else Turn_Target=0;
//	
//    if(Turn_Target>Turn_Amplitude)  Turn_Target=Turn_Amplitude;    //===转向速度限幅
//	  if(Turn_Target<-Turn_Amplitude) Turn_Target=-Turn_Amplitude;
//		if(Flag_Qian==1||Flag_Hou==1)  Kd=0.5;        
//		else Kd=0;   //转向的时候取消陀螺仪的纠正 有点模糊PID的思想
//  	//=============转向PD控制器=======================//
//		Turn=-Turn_Target*Kp -gyro*Kd;                 //===结合Z轴陀螺仪进行PD控制
//	  return Turn;
//}
