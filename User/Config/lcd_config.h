#ifndef LCD_CONFIG_H_INCLUDED
#define LCD_CONFIG_H_INCLUDED

#include "system.h"
#include "stick.h"
#include "spi.h"
#include "stdio.h"

#define DEV_RST_PIN PDout(11)		//PD11
#define DEV_DC_PIN	PDout(12)		//PD12
#define DEV_CS_PIN	PBout(6)		//PB6
#define DEV_BL_PIN	TIM3->CCR2	//PC7

#define BLIGHT_MIN 100
#define BLIGHT_MAX 10240

/**
 * SPI
**/
#define DEV_SPI_WRITE(_dat)  DEV_SPI_WRite(_dat);

/**
 * delay x ms
**/
void DEV_Delay_ms(uint16_t xms );

/**
 * PWM_BL
**/
#define DEV_Set_PWM(_Value)     DEV_BL_PIN= _Value


void HWatchSPI2_DMA_Send16(UWORD *ptdate,UWORD len);
void HWatchSPI2_DMA_Send8(UBYTE ptdate,UWORD len);
void DEV_SPI_WRite(UBYTE _dat);
int DEV_Module_Init(void);
void DEV_Module_Exit(void);

#endif // LCD_CONFIG_H_INCLUDED
#ifndef LCD_CONFIG_H_INCLUDED
#define LCD_CONFIG_H_INCLUDED

#include "system.h"
#include "stick.h"
#include "spi.h"
#include "stdio.h"

#define DEV_RST_PIN PDout(11)		//PD11
#define DEV_DC_PIN	PDout(12)		//PD12
#define DEV_CS_PIN	PBout(6)		//PB6
#define DEV_BL_PIN	TIM3->CCR2	//PC7

#define BLIGHT_MIN 100
#define BLIGHT_MAX 10240

/**
 * SPI
**/
#define DEV_SPI_WRITE(_dat)  DEV_SPI_WRite(_dat);

/**
 * delay x ms
**/
void DEV_Delay_ms(uint16_t xms );

/**
 * PWM_BL
**/
#define DEV_Set_PWM(_Value)     DEV_BL_PIN= _Value


void HWatchSPI2_DMA_Send16(UWORD *ptdate,UWORD len);
void HWatchSPI2_DMA_Send8(UBYTE ptdate,UWORD len);
void DEV_SPI_WRite(UBYTE _dat);
int DEV_Module_Init(void);
void DEV_Module_Exit(void);

#endif // LCD_CONFIG_H_INCLUDED
