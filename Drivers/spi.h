#ifndef SPI_H_INCLUDED
#define SPI_H_INCLUDED

#include "system.h"
#include "stick.h"
//++++++++++++++++++++
//W25Q16 --> SPI1
//CLK --> SPI1_SCK  -- PB3
//DI	--> SPI1_MOSI -- PB5
//DO  --> SPI1_MISO -- PB4
//++++++++++++++++++++
//LCD  --> SPI2
//PB13 --> SPI2_SCK
//PB14 --> SPI2_MISO
//PB15 --> SPI2_MOSI
//++++++++++++++++++++

void HWatchSPI_Init(void);
void HWatchSPI1_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 HWatchSPI1_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
void HWatchSPI2_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 HWatchSPI2_ReadWriteByte(u8 TxData);//SPI总线读写一个字节

#endif // SPI_H_INCLUDED
