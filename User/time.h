#ifndef __TIME_H
#define	__TIME_H

#include "stm32f10x.h"

void delay_us(u32 us);
void delay_ms(u32 ms);

void Time_Counter_Config(void);
void COUNTER_START(void);
u32 COUNTER_STOP(void);

#endif /* __TMIE_H */
