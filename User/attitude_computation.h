#ifndef __ATTITUDE_COMPUTATION_H
#define	__ATTITUDE_COMPUTATION_H

#include "stm32f10x.h"

#include "include.h"

#include "bsp_mpu6050.h"
#include "mpu6050_measure.h"
#include "kalman.h"

//姿态输出
extern xyz_f_t Gyro;
extern xyz_f_t Angle;
extern float angle0;	//初始偏移量校正值，单位是度

void Attitude_Init(void);
void Attitude_sensor_Update(u32 T);
void Attitude_sensor_Read(u32 T);

#endif /* __ATTITUDE_COMPUTATION_H */
