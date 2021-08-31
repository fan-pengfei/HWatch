#include "stm32f4xx.h"
#include "system.h"
#include "stick.h"

#include "iic.h"
#include "usart.h"
#include "adc.h"
#include "spi.h"

#include "drivers.h"
#include "GUI_Paint.h"
#include "gui_manager.h"

//TIM4(1ms)
#define TIM4_PER (10-1)		
#define TIM4_PSC (8400-1)		//0.1ms

void HWatch_Debug(void);
void LCD_1in28_test(void);
void GPS_Test(void);
void TIM4_Init(void);
void HWatch_Logo(void);

void SetUp()
{
	SysTick_Init(168);
	//216M
	//HSI_SetSysClock(16, 432, 2, 9);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	USART1_Init(460800);
	HT2828_Init();
	
	printf("HWatch device init...\n");
	
	IIC_Init();
	HWatchADC_Init();
	HWatchSPI_Init();
	
	//传感器模块初始化
	LED_Init();
	HWatchBeep_Init();
	HDC1080_Init(); 
	DS1302_Init();	
	//DS1302_SetTime(0,1,1,0,50,50,7);
	PulseSensor_Init();
	W25QXX_Init(); 
	
	//配置初始化
	HWatchDevice_Init();
	HWatchDevice_Printf();
	
	//欢迎界面
	DEV_Module_Init();	//LCD引脚初始化
	LCD_1IN28_SetBackLight(g_config.hwatch_config.blight_level << 10);
	LCD_1IN28_Init(VERTICAL);
	
	//HWatch_Logo();
	
	printf("LCD clear begin\n");
	LCD_1IN28_Clear(BLACK);
	printf("LCD clear ok\n");
	
	//GUI初始化
	GUI_ManagerInit();
	
	//开启定时器4
	TIM4_Init();
	
	//初始化完成
	printf("HWatch system running...\n");
	
}

void Loop()
{
	
	for(;;)
	{
		GUI_TaskRun();
	}
}

int main()
{
	SetUp();
	Loop();
}

/*+++++++++++++++++++++++++++TIM4++++++++++++++++++++++++++++++*/
void TIM4_Init()
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);						//使能TIM4时钟
	
	TIM_TimeBaseInitStructure.TIM_Period=TIM4_PER;   							//自动装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=TIM4_PSC; 						//分频系数
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //设置向上计数模式
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); 								//开启定时器中断
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;					//定时器中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;	//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;				//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	
	
	TIM_Cmd(TIM4,ENABLE); //使能定时器
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update))
	{
		GUI_TickRun();
	}
	
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update);	
}

void HWatch_Logo()
{
	LCD_1IN28_Clear(BLACK);

	Paint_NewImage(LCD_1IN28_WIDTH,LCD_1IN28_HEIGHT, 0, BLACK);
	Paint_SetClearFuntion(LCD_1IN28_Clear);
	Paint_SetDisplayFuntion(LCD_1IN28_DrawPaint);
	Paint_Clear(BLACK);
	Delay_Ms(1000);
	
	Paint_DrawCircle(120,120, 120, BLUE ,DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
  Paint_DrawLine  (120, 0, 120, 12,GREEN ,DOT_PIXEL_4X4,LINE_STYLE_SOLID);
  Paint_DrawLine  (120, 228, 120, 240,GREEN ,DOT_PIXEL_4X4,LINE_STYLE_SOLID);
  Paint_DrawLine  (0, 120, 12, 120,GREEN ,DOT_PIXEL_4X4,LINE_STYLE_SOLID);
  Paint_DrawLine  (228, 120, 240, 120,GREEN ,DOT_PIXEL_4X4,LINE_STYLE_SOLID);
	
  //Paint_DrawImage(gImage_70X70, 85, 25, 70, 70); 
  //Paint_DrawString_CN(56,140, "武汉加油，河南加油，中国加油", &Font24CN,BLACK,  WHITE);
  Paint_DrawString_EN(80, 50, "HWatch",&Font20,  BLACK, BRED); 
	
	Paint_DrawLine  (120, 120, 70, 70,YELLOW ,DOT_PIXEL_3X3,LINE_STYLE_SOLID);
	Paint_DrawLine  (120, 120, 176, 64,BLUE ,DOT_PIXEL_3X3,LINE_STYLE_SOLID);
	Paint_DrawLine  (120, 120, 120, 210,RED ,DOT_PIXEL_2X2,LINE_STYLE_SOLID);
	
	Delay_Ms(1000);
	LCD_1IN28_Clear(BLACK);
}

