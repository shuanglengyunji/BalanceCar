#ifndef __BSP_MOTOR_H
#define	__BSP_MOTOR_H

#include "stm32f10x.h"

void Motor_Init(void);
void Left_Motor(s16 speed);
void Right_Motor(s16 speed);

//extern u8 Direction_Left,Rirection_Right;

#endif /* __BSP_MOTOR_H */

