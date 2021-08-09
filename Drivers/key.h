#ifndef KEY_H_INCLUDED
#define KEY_H_INCLUDED

#include "system.h"
#include "stick.h"

#include "gui_inc.h"

#define KEY_UP_PIN 		GPIO_Pin_0
#define KEY_DOWN_PIN 	GPIO_Pin_5
#define KEY_LEFT_PIN	GPIO_Pin_6
#define KEY_RIGHT_PIN	GPIO_Pin_7
#define KEY_A_PIN			GPIO_Pin_8
#define KEY_B_PIN			GPIO_Pin_9

#define KEY_PORT		(GPIOE)
#define KEY_UP_PORT	(GPIOA)

#define K_UP PAin(0)
#define K_DOWN PEin(5)
#define K_LEFT PEin(6)
#define K_RIGHT PEin(7)
#define K_A			PEin(8)
#define K_B			PEin(9)

//一开始没想加入enter&back按键
//后面只能用down&right代替
//再后来没左右键，只能再加入左右键用A&B命名了。。。
/**
		 up1------------2down(enter)
			/								\
		 /								 \
		/										\
 A\/										 \/B  ==> A--LEFT | B--RIGHT
	 \				watch				 /
	  \										/
		 \								 /
			\								/
	 left3-------------4right(back)
**/

#define KEY_UP 1
#define KEY_DOWN 2
#define KEY_LEFT 3
#define KEY_RIGHT 4
#define KEY_A			5
#define KEY_B			6

void KEY_Init(void);
u8 KEY_Scan(u8 mode);

#endif // KEY_H_INCLUDED
