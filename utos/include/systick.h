#ifndef __SYSTICK_H
#define __SYSTICK_H


#include "stm32f10x.h"


void SysTick_Init(void);
void udelay(volatile u32 nTime);         // ��λ1us


#define Delay_ms(x) udelay(1000*x)	 //��λms


#endif /* __SYSTICK_H */
