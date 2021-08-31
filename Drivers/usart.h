#ifndef USART_H_INCLUDED
#define USART_H_INCLUDED

#include "system.h"
#include "stdio.h"
#include <string.h>

#include "lcd_1inch28.h"

//USART1
//PA9: 	TX
//PA10: RX

void USART1_Init(u32 bound);
void USART2_Init(u32 bound);
void Usart1_Dma_Send(void *data,uint16_t size);

#endif // USART_H_INCLUDED
