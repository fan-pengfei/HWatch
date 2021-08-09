#ifndef LEC_H_INCLUDED
#define LEC_H_INCLUDED

#include "system.h"

#define LED_OFF 1
#define LED_ON  0

#define led1 PAout(6) //PA6 //D2
#define led2 PAout(7) //PA7 //D3

void LED_Init(void);

#endif // LEC_H_INCLUDED
