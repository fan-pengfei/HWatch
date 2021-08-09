#include "iic.h"
#include "stick.h"

void IIC_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);			//使能 GPIOB 时钟
	
	//GPIOB8,B9初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;							//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;							//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);										//初始化
	IIC_SCL=1;
	IIC_SDA=1;	
}

void SDA_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//GPIOB9初始化设置
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;					//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;				//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);								//初始化
}

void SDA_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//GPIOB9初始化设置
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;					//输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);								//初始化
}

void IIC_Start(void)
{
	SDA_OUT();     	//sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	Delay_Us(5);
 	IIC_SDA=0;			//START:when CLK is high,DATA change form high to low 
	Delay_Us(6);
	IIC_SCL=0;			//钳住I2C总线，准备发送或接收数据 
}	

void IIC_Stop(void)
{
	SDA_OUT();			//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;			//STOP:when CLK is high DATA change form low to high
 	IIC_SCL=1; 
	Delay_Us(6); 
	IIC_SDA=1;			//发送I2C总线结束信号
	Delay_Us(6);							   	
}

u8 IIC_Wait_Ack(void)
{
	u8 tempTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;
	Delay_Us(1);	   
	IIC_SCL=1;
	Delay_Us(1);	 
	while(READ_SDA)
	{
		tempTime++;
		if(tempTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;		//时钟输出0 	   
	return 0;  
} 

void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	Delay_Us(2);
	IIC_SCL=1;
	Delay_Us(5);
	IIC_SCL=0;
}
	    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	Delay_Us(2);
	IIC_SCL=1;
	Delay_Us(5);
	IIC_SCL=0;
}	
	  
void IIC_Send_Byte(u8 txd)
{                        
	u8 t;   
	SDA_OUT(); 	    
	IIC_SCL=0;								//拉低时钟开始数据传输
	for(t=0;t<8;t++)
	{              
		if((txd&0x80)>0) 				//0x80  1000 0000
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		Delay_Us(2);   					//对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		Delay_Us(2); 
		IIC_SCL=0;	
		Delay_Us(2);
	}	 
} 

u8 IIC_Read_Byte(u8 ack)
{
	u8 i,receive=0;
	SDA_IN();									//SDA设置为输入
	for(i=0;i<8;i++ )
	{
		IIC_SCL=0; 
		Delay_Us(2);
		IIC_SCL=1;
		receive<<=1;
		if(READ_SDA)receive++;   
		Delay_Us(1); 
	}					 
	if (!ack)
			IIC_NAck();						//发送nACK
	else
			IIC_Ack(); 						//发送ACK   
	return receive;
}






