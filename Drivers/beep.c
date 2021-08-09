#include "beep.h"

void HWatchBeep_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体变量
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能端口A时钟
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT; //输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4;//管脚设置PC4
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;//速度为100M
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_DOWN;//上拉
	GPIO_Init(GPIOC,&GPIO_InitStructure); //初始化结构体
	//GPIO_SetBits(GPIOC,GPIO_Pin_4);
	beep = BEEP_OFF;
}

void HWatchBeep_Once(u16 t)
{
	beep = BEEP_ON;
	Delay_Ms(t);
	beep = BEEP_OFF;
}







