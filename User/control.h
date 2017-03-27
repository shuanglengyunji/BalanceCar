#ifndef __CONTROL_H
#define	__CONTROL_H

#include "stm32f10x.h"

extern float kpb,kib,kdb;
extern float kps,kis,kds;

void Control_Init(void);
void Balance_Control(float Angle_y, float Gyro_y,   s16 *Control_Out_Left,s16 *Control_Out_Right,float Expect_Angle_y);
void Speed_Control  (s16 Speed_Left,s16 Speed_Right,s16 *Control_Out_Left,s16 *Control_Out_Right,float Expect_Speed  );

#endif /* __CONTROL_H */
