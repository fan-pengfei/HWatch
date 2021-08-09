#ifndef PULSESENSOR_H_INCLUDED
#define PULSESENSOR_H_INCLUDED

#include "system.h"
#include "stick.h"
#include "adc.h"
#include "led.h"
#include "stdio.h"

//信号线: PA2 -- ADC采集
//TIM4定时器

#define PULSESENSOR_EN_PIN PAout(3) //PA3

typedef struct PulseData{
	u16 ibi;
	u16 bpm;
	u16 sig;
}_PulseData;

typedef void(*HandlerCallback)(_PulseData *p);

extern _PulseData HWatch_pulse;

void PulseSensor_Init(void);
void PulseSensor_Start(void);
void PulseSensor_Stop(void);
BOOL PulseSensor_IsRun(void);
void PulseSensorReg(HandlerCallback handler);

void PulseSensor_Displaywave(void);
void PulseSensorReg(HandlerCallback handler);

void PulseSensor_Update(void);

#endif // PULSESENSOR_H_INCLUDED
