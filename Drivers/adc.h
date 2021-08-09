#ifndef ADC_H_INCLUDED
#define ADC_H_INCLUDED

#include "system.h"
#include "stdio.h"
#include "stick.h"
#include "utils.h"

#define LEAKAGE_0  		    ADC_Channel_2		//心率传感器ADC通道
#define LEAKAGE_1  		    ADC_Channel_16	//MCU内部温度ADC通道

typedef struct HWatchADC_VAL{
	u32 adc_sys_temp;
	u32 adc_pulse;
}_HWatchADC_Val;

extern _HWatchADC_Val HWatch_adc;

void HWatchADC_Init(void);

#endif // ADC_H_INCLUDED
