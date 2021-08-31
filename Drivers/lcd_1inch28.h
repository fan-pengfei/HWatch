#ifndef LCD_1INCH28_H_INCLUDED
#define LCD_1INCH28_H_INCLUDED

/*****************************************************************************
* | File      	:   LCD_1inch28.h
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master 
*                and enhance portability
*----------------
* |	This version:   V1.0
* | Date        :   2020-12-06
* | Info        :   Basic version
*
******************************************************************************/
	
#include "lcd_config.h"
#include <stdint.h>

#include <stdlib.h>		//itoa()
#include <stdio.h>


#define LCD_1IN28_HEIGHT 240
#define LCD_1IN28_WIDTH 240


#define HORIZONTAL 0
#define VERTICAL   1

#define LCD_1IN28_SetBacklight(Value) DEV_SetBacklight(Value) 

#define LCD_1IN28_CS_0	    (DEV_CS_PIN = 0)
#define LCD_1IN28_CS_1	    (DEV_CS_PIN = 1)
	                        
#define LCD_1IN28_RST_0	  	(DEV_RST_PIN = 0)
#define LCD_1IN28_RST_1	  	(DEV_RST_PIN = 1)
	                        
#define LCD_1IN28_DC_0	   	(DEV_DC_PIN = 0)
#define LCD_1IN28_DC_1	   	(DEV_DC_PIN = 1) 

typedef struct{
	UWORD WIDTH;
	UWORD HEIGHT;
	UBYTE SCAN_DIR;
}LCD_1IN28_ATTRIBUTES;
extern LCD_1IN28_ATTRIBUTES LCD_1IN28;

typedef struct{
	UBYTE disp_flush;
	UBYTE disp_play;
	UWORD *disp_buf;
	UWORD disp_size;
	UWORD disp_row;
}LCD_1IN28_DISP;
extern LCD_1IN28_DISP LCD_Disp;
/********************************************************************************
function:	
			Macro definition variable name
********************************************************************************/
void LCD_1IN28_Init(UBYTE Scan_dir);
void LCD_1IN28_Clear(UWORD Color);
void LCD_1IN28_Display(UWORD *Image);
void LCD_1IN28_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void LCD_1IN28_DisplayPoint(UWORD X, UWORD Y, UWORD Color);

void LCD_1IN28_DrawPaint(UWORD x, UWORD y, UWORD Color);
void LCD_1IN28_SetBackLight(UWORD Value);

#endif // LCD_1INCH28_H_INCLUDED
