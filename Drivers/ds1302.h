#ifndef DS1302_H_INCLUDED
#define DS1302_H_INCLUDED

#include "system.h"
#include "stick.h"

//PE12: RST
//PE13:	DAT
//PE14:	CLK

#define IN 	0
#define OUT 1

#define DS1302_IN		PEin(13)
#define DS1302_OUT	PEout(13)
#define DS1302_RST	PEout(12)
#define DS1302_CLK	PEout(14)


#define SEC             0x40
#define MIN             0x41
#define HR              0x42
#define DATE            0x43
#define MONTH           0x44
#define DAY             0x45
#define YEAR            0x46
#define CONTROL         0x47
#define TRACKLE_CHARGER 0x48
#define CLOCK_BURST     0x5F
#define RAM0            0x60
#define RAM1            0x61
#define RAM30           0x7E
#define RAM_BURST       0x7F

typedef struct DS1302_Time{
	u8 year;
	u8 month;
	u8 date;
	u8 hour;
	u8 min;
	u8 sec;
	u8 week;
}_DS1302_t;

extern _DS1302_t HWatch_Time;
extern const char* weeks[];

void DS1302_Init(void);
void DS1302_ReadReg(u8 addr, u8 *value);
void DS1302_GetYear(u8 *year);
void DS1302_GetMonth(u8 *month);
void DS1302_GetDate(u8 *date);
void DS1302_GetHour(u8 *hour);
void DS1302_GetMinite(u8 *minute);
void DS1302_GetSecond(u8 *second);
void DS1302_SetTime(u8 yr, u8 mon, u8 date, u8 hr, u8 min, u8 sec, u8 day);
void DS1302_GetTime(void);

void DS1302_SetYear(u8 a);
void DS1302_SetMon(u8 b);
void DS1302_SetDay(u8 c);
void DS1302_SetHour(u8 d);
void DS1302_SetMin(u8 e);
void DS1302_SetSec(u8 f);
void DS1302_SetWeek(u8 g);

#endif // DS1302_H_INCLUDED
