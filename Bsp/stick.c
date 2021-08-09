#include "stick.h"

static u8  fac_us=0;							//us延时倍乘数
static u16 fac_ms=0;							//ms延时倍乘数

void SysTick_Init(u8 SYSCLK)
{
 	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
	fac_us=SYSCLK/8;								//SYSCLK的8分频 保存1us所需的计数次数
	fac_ms=(u16)fac_us*1000;				//每个ms需要的systick时钟数
}

void Delay_Us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 						//时间加载
	SysTick->VAL=0x00;        						//清空计数器
	SysTick->CTRL|=0x01 ; 								//开始倒数
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达
	SysTick->CTRL&=~0x01; 								//关闭计数器
	SysTick->VAL =0X00;       						//清空计数器
}

void delay_nms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;				//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           				//清空计数器
	SysTick->CTRL|=0x01 ; 								//开始倒数 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	SysTick->CTRL&=~0x01; 								//关闭计数器
	SysTick->VAL =0X00;     		  				//清空计数器	  	    
}

void Delay_Ms(u16 nms)
{
	u8 repeat=nms/540;		//这里用540,是考虑到某些客户可能超频使用,
												//比如超频到248M的时候,delay_nms最大只能延时541ms左右了
	u16 remain=nms%540; 
	while(repeat)
	{
		delay_nms(540);
		repeat--;
	}
	if(remain)delay_nms(remain);
}

/*
 * m: VCO输入时钟，分频因子，2~63
 * n: VCO输出时钟，倍频因子，192~432
 * p: PLLCLK时钟分频因子，2、4、6、8
 * q: OTG FS,SDIO,RNG时钟分频因子，4~15
 * SYSCLK=HCLK=168M,PCLK2=HCLK/2=84M,PCLK1=HCLK/4=42M
 * 168M: HSI_SetSysClock(16, 336, 2, 7); (16/16)*336/2
 * 216M: HSI_SetSysClock(16, 432, 2, 9);
 * 240M: HSI_SetSysClock(8, 240, 2, 9);
 * PLL_VCO  = (HSE / PLL_M)* PLL_N
 * SYSCLK  =  PLL_VCO/ PLL_P
 *
 */

void HSI_SetSysClock(uint32_t m, uint32_t n, uint32_t p, uint32_t q)	
{
  __IO uint32_t HSIStartUpStatus = 0;
	
  RCC_DeInit();
  
	RCC_HSICmd(ENABLE);
	
	HSIStartUpStatus = RCC->CR & RCC_CR_HSIRDY;
	
  if (HSIStartUpStatus == RCC_CR_HSIRDY)
  {
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;
		
		// HCLK = SYSCLK / 1
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		
		// PCLK2 = HCLK / 2
		RCC_PCLK2Config(RCC_HCLK_Div2);
		
		// PCLK1 = HCLK / 4
		RCC_PCLK1Config(RCC_HCLK_Div4);
		
		//超频设置
		RCC_PLLConfig(RCC_PLLSource_HSI, m, n, p, q);
		
		RCC_PLLCmd(ENABLE);
  
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }   

/*-----------------------------------------------------*/
//	开启OVER-RIDE模式，以能达到更高频率（F429）
//    PWR->CR |= PWR_CR_ODEN;
//    while((PWR->CSR & PWR_CSR_ODRDY) == 0)
//    {
//    }
//    PWR->CR |= PWR_CR_ODSWEN;
//    while((PWR->CSR & PWR_CSR_ODSWRDY) == 0)
//    {
//    }      
		//配置FLASH预取指令缓存、数据缓存、等待状态
    FLASH->ACR = FLASH_ACR_PRFTEN 
		            | FLASH_ACR_ICEN 
		            |FLASH_ACR_DCEN 
		            |FLASH_ACR_LATENCY_5WS;
/*-----------------------------------------------------*/
		
		//PLL稳定之后，将PLL时钟切换未系统时钟SYSCLK
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    //读取时钟切换状态位，确保PLLCLK被选为系统时钟
    while (RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  else
  { // HSI启动错误
    while (1)
    {
    }
  }
}

uint32_t SysClockGet(void)
{
  uint32_t    PLLM = 0, PLLN = 0, PLLP = 0, PLLSRC = 0;
       
	if((RCC->CFGR & RCC_CFGR_SWS ) == RCC_CFGR_SWS_HSE) 
		return  HSE_VALUE;
	else if((RCC->CFGR & RCC_CFGR_SWS ) == RCC_CFGR_SWS_HSI) 
		return  HSI_VALUE;
	else                
	{
		PLLM 			= RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
		PLLN    	= ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN)>>6);
		PLLP    	= ((((RCC->PLLCFGR & RCC_PLLCFGR_PLLP)>>16)+1)<<1);
		PLLSRC  	= RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC;
		if(PLLSRC == 0)        
			return (((HSI_VALUE * PLLN) / PLLM )/ PLLP);
		else                                                
			return (((HSE_VALUE * PLLN) / PLLM )/ PLLP);
	}
}
