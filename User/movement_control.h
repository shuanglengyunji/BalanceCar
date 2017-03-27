#ifndef __MOVEMENT_CONTROL_H
#define	__MOVEMENT_CONTROL_H

#include "stm32f10x.h"

void Speed_OutPut(s32 Speed_Left,s32 Speed_Right);
void Get_Speed(float * Speed_Left_CM_S,float * Speed_Right_CM_S,u32 T);	//这个速度从码盘获得，所以用u32表示，不涉及方向

#endif /* __MOVEMENT_CONTROL_H */
