#ifndef __INCLUDE_H
#define	__INCLUDE_H

#include "stm32f10x.h"

#define PI 3.14159265

#define ABS(x) 						( (x)>0?(x):-(x) )
#define LIMIT( x,min,max )			( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )
#define _MIN(a, b) 					((a) < (b) ? (a) : (b))
#define _MAX(a, b) 					((a) > (b) ? (a) : (b))

//typedef struct 
//{
//	char x;
//	char y;
//	char z;
//}xyz_char_t;

typedef struct 
{
	s16 x;
	s16 y;
	s16 z;
}xyz_s16_t;

typedef struct 
{
	float x;
	float y;
	float z;
}xyz_f_t;

typedef struct 
{
	s32 x;
	s32 y;
	s32 z;
}xyz_s32_t;

#endif /* __INCLUDE_H */
