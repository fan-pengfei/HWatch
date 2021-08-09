#include "adc.h"

/**
 * DMA2 - ADC1
 * ADC连续转换，DMA循环存储
 * Ch0:
 *		ADC12_IN15 -- PC5 (采集MCU内部温度电压值)
 *		ADC123_IN2 -- PA2 (采集pulsesensor电压值)
 *
 * 参考: 
 *		https://blog.csdn.net/u014470361/article/details/82315161
 *		https://blog.csdn.net/dianzishi123/article/details/89155700
 */

/**
 * 2通道, 每个通道采样次数N
 */
#define N 	12
#define CH 	2
#define BUFFER_SIZE (N * CH);

_HWatchADC_Val HWatch_adc;

static volatile u16 sHWatchADC_Buf[N][CH];

void ADC_GPIO_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                   
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}

void ADC_DMA_Configuration()
{
	DMA_InitTypeDef DMA_InitStructure;
	
	//使能时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
	DMA_DeInit(DMA2_Stream0);
	while(DMA_GetCmdStatus(DMA2_Stream0)!=DISABLE);
	
	//配置DMA
	DMA_InitStructure.DMA_Channel = DMA_Channel_0;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(ADC1->DR));							//外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)(&sHWatchADC_Buf);							//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; 										//传输方向: 外设->内存
	DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;															//传输长度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;						//外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;											//内存地址递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//半字
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;					//数据宽度
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;															//循环传输
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;													//优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream0, &DMA_InitStructure);

	DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TC); 
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);

	DMA_Cmd(DMA2_Stream0, ENABLE);
}

void ADC_Configuration()
{
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	
	//使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//ADC配置
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;              //浏览模式(多通道)         
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;        //连续转换        
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //禁止触发检测，使用软件触发
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;    //数据右对齐         
  ADC_InitStructure.ADC_NbrOfConversion = CH;            		//转换通道数		             
  ADC_Init(ADC1, &ADC_InitStructure);		

	//通道配置
	ADC_RegularChannelConfig(ADC1, LEAKAGE_0, 1, ADC_SampleTime_480Cycles);
	ADC_RegularChannelConfig(ADC1, LEAKAGE_1, 2, ADC_SampleTime_144Cycles);
	
	//使能内部温度检测
	ADC_TempSensorVrefintCmd(ENABLE);
	
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);			//使能DMA功能
	ADC_Cmd(ADC1, ENABLE); 				//使能ADC
	
	ADC_SoftwareStartConv(ADC1);
}

static void ADC_NvicInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//DMA中断
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;      
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           
	NVIC_Init(&NVIC_InitStructure);  
}

void HWatchADC_Init()
{
	ADC_GPIO_Configuration();
	ADC_DMA_Configuration();
	ADC_Configuration();
	ADC_NvicInit();
}

void HWatchADC_Setval()
{
	u32 adc_temp = 0;
	u32 adc_pulse = 0;
	u32 adc_temp_arr[N];
	u32 adc_pulse_arr[N];
	u8 i;
	u8 n = N-1;
	
	for(i=0;i<N;i++)
	{
		adc_pulse_arr[i] = sHWatchADC_Buf[i][0];
		adc_temp_arr[i] = sHWatchADC_Buf[i][1];
	}
	
	//排序(升序)
	QSortInt(adc_pulse_arr, N);
	QSortInt(adc_temp_arr, N);
	
	//去最大最小做平均
	for(i=1;i<n;i++)
	{
		adc_pulse += adc_pulse_arr[i];
		adc_temp += adc_temp_arr[i];
	}
	
	n -= 1;
	adc_pulse /= n;
	adc_temp /= n;
	
	HWatch_adc.adc_sys_temp = adc_temp;
	HWatch_adc.adc_pulse = adc_pulse;

}

void DMA2_Stream0_IRQHandler(void)  
{
	if(DMA_GetFlagStatus(DMA2_Stream0,DMA_FLAG_TCIF0)!=RESET)
	{
		//停止转换
		ADC_Cmd(ADC1, DISABLE);
		DMA_ClearFlag(DMA2_Stream0,DMA_FLAG_TCIF0);
		
		//处理数据
		HWatchADC_Setval();
	
		//开启转换
		ADC_Cmd(ADC1, ENABLE);
		ADC_SoftwareStartConv(ADC1);
	}
}


