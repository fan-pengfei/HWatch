#ifndef STICK_H_INCLUDED
#define STICK_H_INCLUDED

#include "system.h"

void SysTick_Init(u8 SYSCLK);
void Delay_Us(u32 nus);
void Delay_Ms(u16 nms);
void HSI_SetSysClock(uint32_t m, uint32_t n, uint32_t p, uint32_t q);

#endif // STICK_H_INCLUDED
