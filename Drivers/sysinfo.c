#include "sysinfo.h"

void calc_sys_temp(u16 adcx, float *ptemp)
{
	
	float temperate =(float)adcx*(3.3/4096);		//转换电压值
	temperate = (temperate - 0.76)/0.0025+25;		//转换温度值

	*ptemp = temperate;
}

float HWatchSystemp_Get()
{
	
	u16 adc_val[2];
	float temp = 0;
	
	calc_sys_temp(HWatch_adc.adc_sys_temp, &temp);
	
	return temp;
}

uint16_t HWatchSysFlash_Get(void)
{

   return (*(__IO u16*)(0x1FFF7A22));
   // return (*(__IO u32*)(0x1FFF7A20))>>16;
}

ChipID HWatchSysId_Get()
{
	ChipID chipid = {0};
	memcpy(&chipid,CPU_ID_BASE_ADDR,sizeof(ChipID));
	
	return chipid;
}

