#include "pulsesensor.h"

#define SAMPLE_PERIOD	20  	// 波形采样周期(ms)
#define BUFF_SIZE		50		// 数据缓冲区大小

static BOOL run_flag = FALSE;

_PulseData HWatch_pulse;
HandlerCallback pulsesensor_handler;

u16 readData, preReadData;		   		// 读取到的 AD 值
u16 timeCount = 0;         // 采样周期计数变量
u16 firstTimeCount = 0;    // 第一个心跳时间
u16 secondTimeCount = 0;   // 第二个心跳时间

/* 发送给上位机的三个量 IBI: 相邻两个心跳的时间，BPM: 心率值， SIG: 脉象图的数值化表示*/
u16 IBI, BPM, SIG;   		

u16 data[BUFF_SIZE] = {0}; // 采样数据缓存
u8 arr_index = 0; 				// 缓存数组下标
u16 max, min, mid;			// 最大、最小及中间值
u16 filter;				// 滤波值

BOOL PULSE = FALSE;				// 有效脉搏
BOOL PRE_PULSE = FALSE;         // 先前有效脉搏

u8 pulseCount = 0;			// 脉搏计数
u32 i;

uint16_t Get_Array_Max(uint16_t * array, uint32_t size);
uint16_t Get_Array_Min(uint16_t * array, uint32_t size);

void sendDataToProcessing(char symbol, int dat )
{
    putchar(symbol);                // symbol prefix tells Processing what type of data is coming
		printf("%d\r\n",dat);						// the data to send culminating in a carriage return
}

void PulseSensor_Init()
{
	//PulseSensor_Start();
	
	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体变量
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能端口A时钟
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT; //输出模式
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;//管脚设置A3
	GPIO_InitStructure.GPIO_Speed=GPIO_Low_Speed;//速度为100M
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;//下拉
	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化结构体
	GPIO_SetBits(GPIOA,GPIO_Pin_3);
	
	PULSESENSOR_EN_PIN = 0;
}

void PulseSensor_Start()
{
	PULSESENSOR_EN_PIN = 1;
	printf("Pulsesensor start\n");
	run_flag = TRUE;
}

void PulseSensor_Stop()
{
	PULSESENSOR_EN_PIN = 0;
	printf("Pulsesensor stop\n");
	run_flag = FALSE;
	led1 = LED_OFF;
}

void PulseSensor_Update()
{
	unsigned int runningTotal;
	int timeDiff;
	
	if(run_flag == FALSE){return;}
	
	preReadData = readData;	        		// 保存前一次值
	readData = HWatch_adc.adc_pulse;		// 读取AD转换值
	
	if ((readData - preReadData) < filter)
	{
		data[arr_index++] = readData;
	}

	if (arr_index >= BUFF_SIZE)
	{	
		arr_index = 0;	// 数组填满，从头再填
	
		//通过缓存数组获取脉冲信号的波峰、波谷值，并计算中间值作为判定参考阈值
		max = Get_Array_Max(data, BUFF_SIZE);
		min = Get_Array_Min(data, BUFF_SIZE);
		mid = (max + min)/2;
		filter = (max - min) / 2;
		
		led1 = !led1; //指示灯
	}
	
	PRE_PULSE = PULSE;	// 保存当前脉冲状态
	PULSE = (readData > mid) ? TRUE : FALSE;  // 采样值大于中间值为有效脉冲
	
	if (PRE_PULSE == FALSE && PULSE == TRUE)  // 寻找到“信号上升到振幅中间位置”的特征点，检测到一次有效脉搏
	{	
		pulseCount++;
		pulseCount %= 2;	 
		
		if(pulseCount == 1) // 两次脉搏的第一次
		{                         	
			firstTimeCount = timeCount;   // 记录第一次脉搏时间
		}
		if(pulseCount == 0)  // 两次脉搏的第二次
		{                             			
			secondTimeCount = timeCount;  // 记录第二次脉搏时间
			timeCount = 0;	
			pulseCount = 0;
			timeDiff = (secondTimeCount - firstTimeCount);
			IBI = timeDiff * SAMPLE_PERIOD; //计算相邻两次脉搏的时间，得到 IBI
			
			if (IBI > 250)
			{
				BPM = 60000 / IBI;  // 通过 IBI 得到心率值 BPM
				
				if (BPM > 200)    //限制BPM最高显示值
					BPM = 0;	 				
				if (BPM < 30)    //限制BPM最低显示值
					BPM=0;
				
				//printf("B%d\r\n", BPM);  // 上位机B数据发送
				//printf("Q%d\r\n", IBI);  // 上位机Q数据发送
				
				HWatch_pulse.bpm = BPM;
				HWatch_pulse.ibi = IBI;
				
				if(NULL != pulsesensor_handler)
				{
					pulsesensor_handler(&HWatch_pulse);
				}
			}
		
		}
	
	}
	
	HWatch_pulse.sig = readData;
	timeCount++;  // 时间计数累加
}

uint16_t Get_Array_Max(uint16_t * array, uint32_t size)
{
	uint16_t max = array[0];
	uint32_t i;
	
	for (i = 1; i < size; i++)
	{
		if (array[i] > max)
			max = data[i];
	}
	
	return max;
}

uint16_t Get_Array_Min(uint16_t * array, uint32_t size)
{
	uint16_t min = array[0];
	uint32_t i;
	
	for (i = 1; i < size; i++)
	{
		if (array[i] < min)
			min = data[i];
	}
	
	return min;
}

void PulseSensor_Displaywave(void)
{
	int pluse_count, pluse_i;
	pluse_count = HWatch_adc.adc_pulse / 25;
	
	for(pluse_i=0;pluse_i<pluse_count;pluse_i++)
	{
		printf("*");
	}
	printf("\n");
}

void PulseSensorReg(HandlerCallback handler)
{
	pulsesensor_handler = handler;
}

BOOL PulseSensor_IsRun()
{
	return run_flag;
}




