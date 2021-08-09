### HWatch 引脚连接说明

##### 1、HT2828-GPS模块 |Air530Z定位模块（未连接），程序暂未使用

```
	E-->NULL
	G-->GND
	R-->PD5 (TX)
	T-->PD6 (RX)
	V-->3.3V
	B-->NULL
	BAUD --> 9600
```

##### 2、HDC1080温湿度模块

```
	GND-->GND
	VCC-->3.3V
	DA -->PB9
	CL -->PB8
```

##### 3、DS1302时钟模块

```
	VCC-->3.3V
	GND-->GND
	CLK-->PE14
	DAT-->PE13
	RST-->PE12
```

##### 4、按键模块

```
	/**
		分布
		up     down
	A					B
		left   right
	**/
	
	KEY_UP	 --> PA0 (另一引脚接3.3V)
	KEY_DOWN --> PE5 (GND)
	KEY_LEFT --> PE6 (GND)
	KEY_RIGHT--> PE7 (GND)
	KEY_A	 --> PE8 (GND)
	KEY_B	 --> PE9 (GND)
```

##### 5、PulseSensor心率检测模块

<img src="Img\pulsesensor_pin.jpg" style="zoom: 80%;" />

```
	SIG --> PA2
	VCC --> 3.3V (PA3)
	GND --> GND
```

##### 6、1.28寸圆形LCD屏幕（SPI2）

```
	VCC --> 3.3V
	GND --> GND
	DIN --> PB15
	CLK --> PB13
	CS  --> PB6
	DC  --> PD12
	RST --> PD11
	BL  --> PC7
```

##### 7、USART1串口（连接PC && ESP8266）

```
	RX --> PA10
	TX --> PA9
	BAUD --> 9600
```

##### 8、W25Q16模块(SPI1)

```
	VCC --> 3.3V
	CS  --> PB0
	DO  --> SPI1_MISO -- PB4
	GND --> GND
	CLK --> SPI1_SCK  -- PB3
	DI	--> SPI1_MOSI -- PB5
	
```

##### 9、有源蜂鸣器

```
	VCC --> 3.3V
	SIG	--> PC4
```

##### 10、串口连接图

​		TX，RX根据实际情况调整

​	![](Img\串口连接图.jpg)

##### 11、定时器

```
	TIM4 --> LVGL TICK (1ms)
	TIM3 --> LCD BackLight pwm
```

