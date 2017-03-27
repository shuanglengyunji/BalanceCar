#ifndef __BSP_ENCODER_H
#define	__BSP_ENCODER_H

#include "stm32f10x.h"
#include "include.h"

void Encoder_Configuration(void);	//编码器初始化函数

s32 Get_Left_Speed(void);	//获取转速
s32 Get_Right_Speed(void);

#endif /* __BSP_ENCODER_H */
