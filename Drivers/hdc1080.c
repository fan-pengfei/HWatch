#include "hdc1080.h"

float calc_temp(u16 data)
{
	return (data / pow(2, 16)) * 165 - 40;
}

float calc_humi(u16 data)
{
	return (data / pow(2, 16)) * 100;
}

void write_data(u8 write_addr, u16 data)
{
	u8 msb, lsb;
	msb = (data & 0xFF00) >> 8;			//高8位	
	lsb = (data & 0x00FF);					//低8位
	IIC_Start();										//起始信号
	IIC_Send_Byte(HDC_1080_ADD);		//发送HDC1080设备地址+写信号(0)
	IIC_Wait_Ack();
	IIC_Send_Byte(write_addr);
	IIC_Wait_Ack();
	IIC_Send_Byte(msb);
	IIC_Wait_Ack();
	IIC_Send_Byte(lsb);
	IIC_Wait_Ack();
	IIC_Stop();
}

u16 read_data(u8 read_addr)
{
	u8 msb, lsb;
	IIC_Start();
	IIC_Send_Byte(HDC_1080_ADD);
	IIC_Wait_Ack();
	IIC_Send_Byte(read_addr);
	IIC_Wait_Ack();
//	Delay_Ms(10);	/* Temp转换时间 + Humi转换时间 = 20ms */
	gui_update_delay(10);
	
	IIC_Start(); 										//起始信号
	IIC_Send_Byte(HDC_1080_ADD+1); 	//发送HDC1080设备地址+读信号(1)
	IIC_Wait_Ack();
	
	msb = IIC_Read_Byte(1);
	lsb = IIC_Read_Byte(1);
	
	IIC_NAck();
	IIC_Stop();
	
	return msb << 8 | lsb;
}

void HDC1080_Init()
{
	IIC_Start();
	IIC_Send_Byte(HDC_1080_ADD);
	IIC_Wait_Ack();
	
	//配置0x02寄存器
	switch(READ_MODE)
	{
		case 0:
			write_data(CONFIGURATION, Config_Readmode_0);
			break;
		case 1:
			write_data(CONFIGURATION, Config_Readmode_1);
			break;
		case 2:
			write_data(CONFIGURATION, Config_Readmode_2);
			break;
		case 3:
			write_data(CONFIGURATION, Config_Readmode_3);
			break;
	}
	
}

u16 HDC1080_ReadConfigation()
{
	return read_data(CONFIGURATION);
}

u16 HDC1080_ReadManufacturerId()
{
	return read_data(MANUFACTURER_ID);
}

u16 HDC1080_ReadDeviceId()
{
	return read_data(DEVICE_ID);
}

float HDC1080_ReadHumidity()
{
	u16 rawH = read_data(HUMIDITY);
	return calc_humi(rawH);
}

float HDC1080_ReadTemperature() 
{
	u16 rawT = read_data(TEMPERATURE);
	return calc_temp(rawT);
}

void HDC1080_ReadTempHumidity(float *temp, float *humi)
{
	u8 data[4];
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;
	
	IIC_Start();
	IIC_Send_Byte(HDC_1080_ADD);
	IIC_Wait_Ack();
	IIC_Send_Byte(TEMPERATURE);
	IIC_Wait_Ack();
//	Delay_Ms(20);	/* Temp转换时间 + Humi转换时间 = 20ms */
	gui_update_delay(20);
	
	IIC_Start(); 										//起始信号
	IIC_Send_Byte(HDC_1080_ADD+1); 	//发送HDC1080设备地址+读信号(1)
	IIC_Wait_Ack();
	
	data[0] = IIC_Read_Byte(1);
	data[1] = IIC_Read_Byte(1);
	data[2] = IIC_Read_Byte(1);
	data[3] = IIC_Read_Byte(1);
	IIC_Stop();
	
	*temp = calc_temp(data[0]<<8|data[1]);
	*humi = calc_humi(data[2]<<8|data[3]);
}












