#ifndef BEEP_H_INCLUDED
#define BEEP_H_INCLUDED

#include "system.h"
#include "stick.h"

#include "gui_inc.h"

#define BEEP_ON  0
#define BEEP_OFF 1

#define beep PCout(4)  //PC4

void HWatchBeep_Init(void);
void HWatchBeep_Once(u16 t);

#endif // BEEP_H_INCLUDED
