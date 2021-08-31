#include "lcd_config.h"

u8 dev_init = 0;

//84M
//TIM3 -- PWM -- CONFIG
void DEV_TIM3_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM3);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      
	GPIO_Init(GPIOC,&GPIO_InitStructure);     

	TIM_TimeBaseStructure.TIM_Prescaler=(100-1);  
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseStructure.TIM_Period=(8400-1);  
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV2; 
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	
	//初始化TIM3 CH2 PWM模式
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	
	TIM_ARRPreloadConfig(TIM3,ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}

void HWatchDMA_Init(void)
{
	//LCD DMA
	DMA_InitTypeDef  DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* DMA2 clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, DISABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	DMA_DeInit(DMA1_Stream4);
	while(DMA_GetCmdStatus(DMA1_Stream4)!=DISABLE);
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(SPI2->DR));
//	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)0;//SPI2_Tx_Buf;
//	DMA_InitStructure.DMA_BufferSize = 57600;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;	//存储器到外设
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream4, &DMA_InitStructure);
 
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
	
	NVIC_EnableIRQ(DMA1_Stream4_IRQn);
	DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);
	
}

void DEV_GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOB,ENABLE); 
	
	//PD11,PD12
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT; 
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11|GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&GPIO_InitStructure); 
	GPIO_SetBits(GPIOD,GPIO_Pin_11|GPIO_Pin_12);
	
	//PB6
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT; 
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB,&GPIO_InitStructure); 
	GPIO_SetBits(GPIOD,GPIO_Pin_6);
	
}

//配置SPI2 DMA传输
void HWatchSPI2_DMA_Send16(UWORD *ptdate,UWORD len)
{
	DMA_Cmd(DMA1_Stream4, DISABLE);
	while(DMA_GetCmdStatus(DMA1_Stream4)!=DISABLE);
	
	DMA1_Stream4->NDTR=len;
	DMA1_Stream4->M0AR=(u32)(ptdate);
	
	DMA_Cmd(DMA1_Stream4, ENABLE);
//	while(DMA_GetFlagStatus(DMA1_Stream4,DMA_FLAG_TCIF4) == RESET);
//	DMA_ClearFlag(DMA1_Stream4,DMA_FLAG_TCIF4);
}

void DEV_Delay_ms(uint16_t xms )
{
	Delay_Ms(xms);
}

void DEV_SPI_WRite(UBYTE _dat)
{
	HWatchSPI2_ReadWriteByte(_dat);
}

int DEV_Module_Init()
{
	if(dev_init == 0)
	{
		DEV_GPIO_Init();
		DEV_TIM3_Init();
		HWatchDMA_Init();
		dev_init = 1;
	}
	
	DEV_DC_PIN = 1;
	DEV_CS_PIN = 1;
	DEV_RST_PIN = 1;
	
	TIM_Cmd(TIM3, ENABLE);
	
	return 0;
}

void DEV_Module_Exit()
{
	DEV_DC_PIN = 0;
	DEV_CS_PIN = 0;
	DEV_RST_PIN = 0;
	
	TIM_Cmd(TIM3, DISABLE);
}

