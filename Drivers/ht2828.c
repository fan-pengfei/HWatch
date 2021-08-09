#include "ht2828.h"


//CMD
const u8 ht2828_control_cmd[4][12] = {
	0xB5,0x62,0x06,0x04,0x04,0x00,0xFF,0x87,0x01,0x00,0x95,0xF7,		//复位
	0xB5,0x62,0x06,0x04,0x04,0x00,0xFF,0xFF,0x02,0x00,0x0E,0x61,		//冷启动
	0xB5,0x62,0x06,0x04,0x04,0x00,0x00,0x00,0x02,0x00,0x10,0x68,		//热启动
	0xB5,0x62,0x06,0x04,0x04,0x00,0x00,0x00,0x08,0x00,0x16,0x74,		//低功耗模式
};

u8 HT2828_Buff[BUFFER_SIZE];
u16 rev_index = 0;

u8 HT2828_RX_Tout = 0;
u8 HT2828_RX_Tstart = 1;

static BOOL run_flag = 0;

//USART2 for gps
//baud 9600
void USART2_Init()
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); 		//使能GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);		//使能USART2时钟
 
	//串口2对应引脚复用映射
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2); //GPIOD5复用为USART2
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); //GPIOD6复用为USART2
	
	//USART2端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6 ; 	//GPIOD5与GPIOD6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;						//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 					//推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 						//上拉
	GPIO_Init(GPIOD,&GPIO_InitStructure); 									//初始化PD5，PD6
	

  //USART2 初始化设置
	USART_InitStructure.USART_BaudRate = 9600;																			//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;											//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;													//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;															//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									//收发模式
	USART_Init(USART2, &USART_InitStructure); 																			//初始化串口2
	
	USART_Cmd(USART2, ENABLE);  															//使能串口2
	
	USART_ClearFlag(USART2, USART_FLAG_TC);
		
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);						//开启相关中断

	//Usart2 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;					//串口2中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;		//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;					//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);														//根据指定的参数初始化VIC寄存器?
}

//串口2接收中断
void USART2_IRQHandler(void)
{
	u8 r;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  		
	{
		r =USART_ReceiveData(USART2);
		
		if(rev_index == 0)
		{
			led2 = LED_ON;
		}
	
		HT2828_RX_Tout = 0;
		HT2828_RX_Tstart = 1;
	
		if(rev_index < BUFFER_SIZE)
		{
//			printf("%d",rev_index);
			HT2828_Buff[rev_index] = r;
			rev_index++;
		}else{
			HT2828_RX_Tout = 2;
		}
		
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC) != SET){}
	} 
	
	//USART_ClearFlag(USART2,USART_FLAG_TC);
}

void HT2828_Init()
{
	USART2_Init();
	//HT2828_Start();
}

void HT2828_Sendcmd(u8 index)
{
	u8 i;
	for(i=0;i<12;i++)
	{
		//u8 c = *(*(pcmd+n)+i);
		u8 c = ht2828_control_cmd[index][i];
		USART_SendData(USART2,c);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
	}
}

void HT2828_Start()
{
	printf("HT2828 start\n");
	HT2828_Sendcmd(CMD_HOTBOOT);
	run_flag = TRUE;
}

void HT2828_Stop()
{
	printf("HT2828 stop\n");
	HT2828_Sendcmd(CMD_LOWPOWER);
	run_flag = FALSE;
}

void HT2828_Reset()
{
	printf("HT2828 reset\n");
	HT2828_Sendcmd(CMD_RESET);
}

void HT2828_Update()
{
	if(run_flag == FALSE){return;}
	
	if(HT2828_RX_Tstart == 1)
	{
		HT2828_RX_Tout++;
		if(HT2828_RX_Tout >= 2)
		{
			led2 = LED_OFF;
			
//			printf("%d\n", rev_index);
//			printf("%s\n", HT2828_Buff);
			memset(HT2828_Buff, 0, sizeof(HT2828_Buff));
			
			rev_index = 0;
			HT2828_RX_Tstart = 0;
		}
	}
	
}



