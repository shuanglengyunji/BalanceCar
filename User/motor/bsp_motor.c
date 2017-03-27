#include "bsp_motor.h"
#include "include.h"

static void TIM3_GPIO_Config(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* 设置TIM3CLK 为 72MHZ */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); 

  /* GPIOA and GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); 

  /*GPIOA Configuration: TIM3 channel 1 and 2 as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		    // 复用推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/**
  * @brief  配置TIM3输出的PWM信号的模式，如周期、极性、占空比
  * @param  无
  * @retval 无
  */
/*
 * TIMxCLK/CK_PSC --> TIMxCNT --> TIMx_ARR --> TIMxCNT 重新计数
 *                    TIMx_CCR(电平发生变化)
 * 信号周期=(TIMx_ARR +1 ) * 时钟周期
 * 占空比=TIMx_CCR/(TIMx_ARR +1)
 */
static void TIM3_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	/* PWM信号电平跳变值 */
	u16 CCR1_Val = 0;        
	u16 CCR2_Val = 0;

	/* Time base configuration */		 
	TIM_TimeBaseStructure.TIM_Period = 999;       //当定时器从0计数到999，即为1000次，为一个定时周期
	TIM_TimeBaseStructure.TIM_Prescaler = 0;	    //设置预分频：不预分频，即为72MHz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1 ;	//设置时钟分频系数：不分频(这里用不到)
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	    //配置为PWM模式1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平
	
	/* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	
	TIM_OCInitStructure.TIM_Pulse = CCR1_Val;	   //设置跳变值，当计数器计数到这个值时，电平发生跳变
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);	 //使能通道1
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	/* PWM1 Mode configuration: Channel2 */
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = CCR2_Val;	  //设置通道2的电平跳变值，输出另外一个占空比的PWM
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);	  //使能通道2
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);

	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);                   //使能定时器3	
}

/**
  * @brief  TIM3 输出PWM信号初始化，只要调用这个函数
  *         TIM3的四个通道就会有PWM信号输出
  * @param  无
  * @retval 无
  */
void TIM3_PWM_Init(void)
{
	TIM3_GPIO_Config();
	TIM3_Mode_Config();	
}

void Motor_Init(void)
{
	//初始化控制电机转向的4个GPIO 	(GPIOB_Pin_15 GPIOB_Pin_14 GPIOB_Pin_13 GPIOB_Pin_12)
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能PB端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_14|GPIO_Pin_13|GPIO_Pin_12;	 //这里需要4个端口，要对着电路板查一下
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;     	 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    	 //50M
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB
	
	//Timer
	TIM3_PWM_Init();
}


/*左右轮脉冲占空比调整函数（用于TIM3）*/
void LC_CHG(u16 ChannelPulse)   //F为需频率 C为占空比 如F=2000，C取值0-1000
{
	TIM_SetCompare2(TIM3,ChannelPulse);    
}

void RC_CHG(u16 ChannelPulse)   //F为需频率 C为占空比 如F=2000，C=0.5
{
	TIM_SetCompare1(TIM3,ChannelPulse);      
}

//************************************************************************************

void Left_Motor(s16 speed) //0 停止 1前进 2后退    speed取值0-1000	//B15 B14 A6
{
	if(speed > 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_15);
		GPIO_ResetBits(GPIOB, GPIO_Pin_14);	
	
	}
	else if(speed < 0)
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_15);
		GPIO_SetBits(GPIOB, GPIO_Pin_14);		
	}	
	else 	//这种情况会造成两个轮子都不转了
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_15);
		GPIO_ResetBits(GPIOB, GPIO_Pin_14);
	}
	
	speed = ABS(speed);				//取绝对值
//	speed = speed*1.1;
	speed = LIMIT(speed,0,999);	//限幅

	LC_CHG(speed);   //F为需频率 C为占空比 如F=2000，C取值0-1000
}

void Right_Motor(s16 speed) //0 停止 1前进 2后退    speed取值0-1000
{
	if(speed > 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_13);
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	}
	else if(speed < 0)
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_13);
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
	}	
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_13);
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	}
	
	speed = ABS(speed);				//取绝对值
//	speed = speed*0.9;
	speed = LIMIT(speed,0,999);		//限幅

	RC_CHG(speed);
}

