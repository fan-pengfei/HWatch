#ifndef SYSINFO_H_INCLUDED
#define SYSINFO_H_INCLUDED

#include "adc.h"
#include <string.h>


#define CPU_ID_BASE_ADDR (u32*)(0x1FFF7A10)

typedef struct
{
	union{
		u8  byte[12];    
		u32 id[3];
	}v;
}ChipID;


float HWatchSystemp_Get(void);
uint16_t HWatchSysFlash_Get(void);
ChipID HWatchSysId_Get();

#endif // SYSTEMP_H_INCLUDED
