#include "ds1302.h"

_DS1302_t HWatch_Time;

const char* weeks[] = {
	"SUN.","MON.","TUE.","WED.","THU.","FRI.","SAT."
};

void DS1302_Init()
{
	GPIO_InitTypeDef GPIO_InitStructre;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);            
	GPIO_InitStructre.GPIO_Pin= GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
	GPIO_InitStructre.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructre.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_Init(GPIOE, &GPIO_InitStructre);
	
	DS1302_RST = 0;
	DS1302_CLK = 0;
	DS1302_OUT = 0;
}

void DS1302_IO_GPIO(u8 FLAG)
{
  GPIO_InitTypeDef GPIO_InitStructre;
	if(FLAG==0x01)
  {
		//配置IO为输出
		GPIO_InitStructre.GPIO_Pin= GPIO_Pin_13;
		GPIO_InitStructre.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructre.GPIO_Mode=GPIO_Mode_OUT;
		GPIO_Init(GPIOE, &GPIO_InitStructre);
  }
	else if(FLAG==0x00)
	{
		//配置IO为输入
		GPIO_InitStructre.GPIO_Pin= GPIO_Pin_13;
		GPIO_InitStructre.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructre.GPIO_Mode=GPIO_Mode_IN;
		GPIO_Init(GPIOE, &GPIO_InitStructre);
	}
}

void DS1302_WriteByte(u8 value) 
{
	u8 i;
	DS1302_IO_GPIO(OUT);
	for (i=0;i<8;i++){
		DS1302_CLK = 0;
		if (value&0x01) {
			DS1302_OUT = 1;
		} else {
			DS1302_OUT = 0;
		}
		
		Delay_Us(1);
		DS1302_CLK = 1;
		Delay_Us(1);
		value >>= 1;
	}
}

void DS1302_ReadByte(u8 *value) 
{
	u8 i;
	DS1302_IO_GPIO(IN);
	for (i=0;i<8;i++) {
		*value >>= 1;
		DS1302_CLK = 1;
		Delay_Us(1);
		DS1302_CLK = 0;
		Delay_Us(1);
		if (DS1302_IN == 1) {
			*value |= 0x80;
		} else {
			*value &= 0x7F;
		}
	}
}

void DS1302_ReadReg(u8 addr, u8 *value) 
{
	DS1302_CLK = 0;
	DS1302_RST = 1;
	DS1302_WriteByte((addr<<1)|0x01);
	DS1302_ReadByte(value);
	DS1302_RST = 0;
}

void DS1302_WriteReg(u8 addr, u8 value) 
{
	DS1302_CLK = 0;
	DS1302_RST = 1;
	DS1302_WriteByte((addr<<1)&0xfe);
	DS1302_WriteByte(value);
	DS1302_CLK = 0;
	DS1302_RST = 0;
}

void DS1302_GetYear(u8 *year) 
{
	u8 value;
	DS1302_ReadReg(YEAR, &value);
	*year = ((value&0xf0)>>4)*10 + (value&0x0f);
}

void DS1302_GetMonth(u8 *month) 
{
	u8 value;
	DS1302_ReadReg(MONTH, &value);
	*month = ((value&0x10)>>4)*10 + (value&0x0f);
}

void DS1302_GetDate(u8 *date) 
{
	u8 value;
	DS1302_ReadReg(DATE, &value);
	*date = ((value&0x30)>>4)*10 + (value&0x0f);
}

void DS1302_GetHour(u8 *hour) 
{
	u8 value;
	DS1302_ReadReg(HR, &value);
	//*hour = ((value&0x10)>>4)*10 + (value&0x0f);
	*hour = ((value)>>4)*10 + (value&0x0f);
}

void DS1302_GetMinite(u8 *minute) 
{
	u8 value;
	DS1302_ReadReg(MIN, &value);
	*minute = ((value&0x70)>>4)*10 + (value&0x0f);
}

void DS1302_GetSecond(u8 *second) 
{
	u8 value;
	DS1302_ReadReg(SEC, &value);
	*second = ((value&0x70)>>4)*10 + (value&0x0f);
}

void DS1302_GetDay(u8 *day)
{
	u8 value;
	DS1302_ReadReg(DAY, &value);
	*day = ((value&0x70)>>4)*10 + (value&0x0f);
}

void DS1302_SetTime(u8 yr, u8 mon, u8 date, u8 hr, u8 min, u8 sec, u8 day) 
{
	DS1302_WriteReg(CONTROL, 0x00);
	DS1302_WriteReg(SEC, 0x80);
	DS1302_WriteReg(YEAR, ((yr/10)<<4)|(yr%10));
	DS1302_WriteReg(MONTH, ((mon/10)<<4)|(mon%10));
	DS1302_WriteReg(DATE, ((date/10)<<4)|(date%10));
	DS1302_WriteReg(HR, ((hr/10)<<4)+(hr%10));
	DS1302_WriteReg(MIN, ((min/10)<<4)|(min%10));
	DS1302_WriteReg(SEC, ((sec/10)<<4)|(sec%10));
	DS1302_WriteReg(DAY, ((day/10)<<4)|(day%10));
	DS1302_WriteReg(CONTROL, 0x80);
}

void DS1302_SetYear(u8 yr)
{
	DS1302_WriteReg(CONTROL, 0x00);
	DS1302_WriteReg(SEC, 0x80);
	DS1302_WriteReg(YEAR, ((yr/10)<<4)|(yr%10));
	DS1302_WriteReg(CONTROL, 0x80);
}

void DS1302_SetMon(u8 mon)
{
	DS1302_WriteReg(CONTROL, 0x00);
	DS1302_WriteReg(SEC, 0x80);
	DS1302_WriteReg(MONTH, ((mon/10)<<4)|(mon%10));
	DS1302_WriteReg(CONTROL, 0x80);
}

void DS1302_SetDay(u8 date)
{
	DS1302_WriteReg(CONTROL, 0x00);
	DS1302_WriteReg(SEC, 0x80);
	DS1302_WriteReg(DATE, ((date/10)<<4)|(date%10));
	DS1302_WriteReg(CONTROL, 0x80);
}

void DS1302_SetHour(u8 hr)
{
	DS1302_WriteReg(CONTROL, 0x00);
	DS1302_WriteReg(SEC, 0x80);
	DS1302_WriteReg(HR, ((hr/10)<<4)+(hr%10));
	DS1302_WriteReg(CONTROL, 0x80);
}

void DS1302_SetMin(u8 min)
{
	DS1302_WriteReg(CONTROL, 0x00);
	DS1302_WriteReg(SEC, 0x80);
	DS1302_WriteReg(MIN, ((min/10)<<4)|(min%10));
	DS1302_WriteReg(CONTROL, 0x80);
}

void DS1302_SetSec(u8 sec)
{
	DS1302_WriteReg(CONTROL, 0x00);
	DS1302_WriteReg(SEC, 0x80);
	DS1302_WriteReg(SEC, ((sec/10)<<4)|(sec%10));
	DS1302_WriteReg(CONTROL, 0x80);
}

void DS1302_SetWeek(u8 day)
{
	DS1302_WriteReg(CONTROL, 0x00);
	DS1302_WriteReg(SEC, 0x80);
	DS1302_WriteReg(DAY, ((day/10)<<4)|(day%10));
	DS1302_WriteReg(CONTROL, 0x80);
}

void DS1302_GetTime()
{
	DS1302_GetYear(&(HWatch_Time.year));
	DS1302_GetMonth(&(HWatch_Time.month));
	DS1302_GetDate(&(HWatch_Time.date));
	DS1302_GetHour(&(HWatch_Time.hour));
	DS1302_GetMinite(&(HWatch_Time.min));
	DS1302_GetSecond(&(HWatch_Time.sec));
	DS1302_GetDay(&(HWatch_Time.week));
}
