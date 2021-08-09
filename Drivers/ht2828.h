#ifndef HT2828_H_INCLUDED
#define HT2828_H_INCLUDED

#include "system.h"
#include "stick.h"
#include "stdio.h"
#include "led.h"
#include <string.h>

#define BUFFER_SIZE 512

//USART2
//PD5: TX
//PD6: RX
//PD7: EN

//只解析GGA,RMC协议

enum HT2828_CMD{
	CMD_RESET = 0,
	CMD_COLDBOOT,
	CMD_HOTBOOT,
	CMD_LOWPOWER
};


void HT2828_Init(void);
void HT2828_Start(void);
void HT2828_Stop(void);
void HT2828_Reset(void);
void HT2828_Update(void);
void HT2828_Sendcmd(u8 index);

#endif // HT2828_H_INCLUDED
