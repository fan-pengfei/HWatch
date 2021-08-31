#include "usart.h"

/**************************/
//USART1 - DMA
//USART1-RX -> DMA2-2
//USART1-TX -> DMA2-7
/**************************/

#define USART_TX_SIZE 1024
#define USART_RX_SIZE 8194 //一行数据长度//RGB565

u8 usart_rx_buf[8194];
u8 usart_tx_buf[1024];

/**
 * 串口重定向
 */
int fputc(int ch,FILE *p)  
{
	USART_SendData(USART1,(u8)ch);	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}

//USART1 GPIO CONFIG
void USART1_GPIO_Init()
{
  //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 		//使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);		//使能USART1时钟
 
	//串口1对应引脚复用映射
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9复用为USART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10复用为USART1
	
	//USART1端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10 ; 	//GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;						//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 					//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 						//上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); 									//初始化PA9，PA10
}

//USART1 DMA CONFIG
void USART1_DMA_Init()
{
	DMA_InitTypeDef DMA_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
	//RX
	DMA_DeInit(DMA2_Stream5);
	while(DMA_GetCmdStatus(DMA2_Stream5)!=DISABLE);
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART1->DR));							//外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)(usart_rx_buf);							//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; 										//传输方向: 外设->内存
	DMA_InitStructure.DMA_BufferSize = USART_RX_SIZE;															//传输长度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;						//外设地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;											//内存地址递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;					//数据宽度
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;															
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;													//优先级
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream5, &DMA_InitStructure);
	
	DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TC); 
	DMA_ITConfig(DMA2_Stream5, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA2_Stream5, ENABLE);
	
	//TX
	DMA_DeInit(DMA2_Stream7);
	while(DMA_GetCmdStatus(DMA2_Stream7)!=DISABLE);
	
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(USART1->DR));
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)(usart_tx_buf);	
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral; 
	DMA_InitStructure.DMA_BufferSize = 0;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream7, &DMA_InitStructure);
	
	DMA_ClearITPendingBit(DMA2_Stream7, DMA_IT_TC);
	DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA2_Stream7, ENABLE);

	USART_DMACmd(USART1, USART_DMAReq_Tx|USART_DMAReq_Rx, ENABLE);
}

//USART1 NVIC CONFIG
void USART1_NVIC_Init()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;					//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;					//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);														//根据指定的参数初始化VIC寄存器、	
	
	//RX
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);		
	
	//TX
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	
}


//USART1 for usb2ttl
void USART1_Init(u32 bound)
{
	USART_InitTypeDef USART_InitStructure;
	
	//GPIO初始化
	USART1_GPIO_Init();
	
	//DMA初始化
	USART1_DMA_Init();
	
	//NVIC初始化
	USART1_NVIC_Init();

  //USART1 初始化
	USART_InitStructure.USART_BaudRate = bound;																			//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;											//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;													//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;															//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									//收发模式
	
	USART_Init(USART1, &USART_InitStructure); 																			//初始化串口1
	USART_Cmd(USART1, ENABLE);  															//使能串口1 
	
	//USART_ITConfig(USART1, USART_IT_TC, ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);						//开启相关中断
	
	
}

//发送数据
void Usart1_Dma_Send(void *data,uint16_t size)
{
	while (DMA_GetCurrDataCounter(DMA2_Stream7));
	//memset(usart_tx_buf, 0, USART_DMA_SIZE);
	memcpy(usart_tx_buf, data,(size > USART_TX_SIZE ? USART_TX_SIZE:size));
	DMA_Cmd(DMA2_Stream7, DISABLE);
	DMA2_Stream7->NDTR=size;
	DMA_Cmd(DMA2_Stream7, ENABLE);
}

//接收中断//RX
void DMA2_Stream5_IRQHandler(void)  
{
	uint16_t len = 0; 
	
	if(DMA_GetITStatus(DMA2_Stream5, DMA_IT_TCIF5) != RESET)
	{
		
		DMA_Cmd(DMA2_Stream5,DISABLE); 
		DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TCIF5);	
		DMA_ClearFlag(DMA2_Stream5,DMA_FLAG_TCIF5);
		DMA_Cmd(DMA2_Stream5,ENABLE); 
		
	}
}

//发送中断//TX
void DMA2_Stream7_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA2_Stream7, DMA_IT_TCIF7) != RESET)
  {	
    DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);
		DMA_Cmd(DMA2_Stream7,DISABLE);  
		USART_ITConfig(USART1,USART_IT_TC,ENABLE);
  }
}

void USART1_IRQHandler(void)                								//串口1中断服务程序
{
//	u8 r;
	u8 tem;
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  		//接收中断
	{
		//r =USART_ReceiveData(USART1);														//(USART1->DR);	//读取接收到的数据
		//while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
		
		DMA_Cmd(DMA2_Stream5,DISABLE);
		
		tem=USART1->SR;
    tem=USART1->DR;
    tem=tem;
	
		u16 len =USART_RX_SIZE-DMA_GetCurrDataCounter(DMA2_Stream5);
		
		if(len == 0 && LCD_Disp.disp_play && !LCD_Disp.disp_flush)
		{
			LCD_Disp.disp_row = usart_rx_buf[0] << 8 | (usart_rx_buf[1] & 0xFF);
			memcpy(LCD_Disp.disp_buf, usart_rx_buf+2, LCD_Disp.disp_size << 1);
			LCD_Disp.disp_flush = 1;
		}
		
		DMA2_Stream5->NDTR=USART_RX_SIZE;
		DMA_Cmd(DMA2_Stream5,ENABLE);
	} 
	USART_ClearFlag(USART1,USART_FLAG_TC);
} 	


