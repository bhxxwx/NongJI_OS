/*
 * keys.c
 *
 *  Created on: 2020年4月13日
 *      Author: WangXiang
 */
#include "UserConfig.h"
#include "keys.h"

NVIC_InitTypeDef NVIC_InitStructure;
void (*e1_left_handler)(void);
void (*e1_right_handler)(void);

void (*e2_left_handler)(void);
void (*e2_right_handler)(void);

/*
 * EC11编码开关初始化
 *
 */
void EC11_init()
{
	EXTI_InitTypeDef EXTI_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	//抢占优先级2
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;			//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能外部中断通道

	nvic_init();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //外部中断，需要使能AFIO时钟
#ifdef E1_1_Poart_A
	        pinModeA(E1_1,INPUT);
	        GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,E1_1_IT);
#define E1_1_check digitalReadA(E1_1)
#endif
#ifdef E1_1_Poart_B
	pinModeB(E1_1, INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, E1_1_IT);
#define E1_1_check digitalReadB(E1_1)
#endif
#ifdef E1_1_Poart_C
	pinModeC(E1_1,INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,E1_1_IT);
#define E1_1_check digitalReadC(E1_1)
#endif

#ifdef E1_2_Poart_A
	pinModeA(E1_2,INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,E1_2_IT);
#define E1_2_check digitalReadA(E1_2)
#endif
#ifdef E1_2_Poart_B
	pinModeB(E1_2, INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, E1_2_IT);
#define E1_2_check digitalReadB(E1_2)
#endif
#ifdef E1_2_Poart_C
	pinModeC(E1_2,INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,E1_2_IT);
#define E1_2_check digitalReaC(E1_2)
#endif

#ifdef E1_3_Poart_A
	pinModeA(E1_3,INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,E1_3_IT);
#define E1_3_check digitalReadA(E1_3)
#endif
#ifdef E1_3_Poart_B
	pinModeB(E1_3,INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,E1_3_IT);
#define E1_3_check digitalReadB(E1_3)
#endif
#ifdef E1_3_Poart_C
	pinModeC(E1_3, INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, E1_3_IT);
#define E1_3_check digitalReadC(E1_3)
#endif

#ifdef E2_1_Poart_A
	pinModeA(E2_1,INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,E2_1_IT);
#define E2_1_check digitalReadA(E2_1)
#endif
#ifdef E2_1_Poart_B
	pinModeB(E2_1, INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, E2_1_IT);
#define E2_1_check digitalReadB(E2_1)
#endif
#ifdef E2_1_Poart_C
	pinModeC(E2_1,INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,E2_1_IT);
#define E2_1_check digitalReadC(E2_1)
#endif

#ifdef E2_2_Poart_A
	pinModeA(E2_2,INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,E2_2_IT);
#define E2_2_check digitalReadA(E2_2)
#endif
#ifdef E2_2_Poart_B
	pinModeB(E2_2, INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, E2_2_IT);
#define E2_2_check digitalReadB(E2_2)
#endif
#ifdef E2_2_Poart_C
	pinModeC(E2_2,INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,E2_2_IT);
#define E2_2_check digitalReadC(E2_2)
#endif

#ifdef E2_3_Poart_A
	pinModeA(E2_3,INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,E2_3_IT);
#define E2_3_check digitalReadA(E2_3)
#endif
#ifdef E2_3_Poart_B
	pinModeB(E2_3, INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, E2_3_IT);
#define E2_3_check digitalReadB(E2_3)
#endif
#ifdef E2_3_Poart_C
	pinModeC(E2_3,INPUT);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,E2_3_IT);
#define E2_3_check digitalReadC(E2_3)
#endif

	EXTI_InitStructure.EXTI_Line = (uint32_t) E1_1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	it_init(E1_1);

	EXTI_InitStructure.EXTI_Line = (uint32_t) E2_1;
	EXTI_Init(&EXTI_InitStructure);
	it_init(E2_1);

	EXTI_InitStructure.EXTI_Line = (uint32_t) E1_3;
	EXTI_Init(&EXTI_InitStructure);
	it_init(E1_3);

	EXTI_InitStructure.EXTI_Line = (uint32_t) E2_3;
	EXTI_Init(&EXTI_InitStructure);
	it_init(E2_3);



}

/*
 * GPIO引脚中断初始化
 * 输入: GPIO_Pin_x
 */
void it_init(uint16_t pin)
{
	if (pin == 0x0001)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;				//使能按键所在的外部中断通道
	}
	else if (pin == 0x0002)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;				//使能按键所在的外部中断通道
	}
	else if (pin == 0x0004)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;				//使能按键所在的外部中断通道
	}
	else if (pin == 0x0008)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;				//使能按键所在的外部中断通道
	}
	else if (pin == 0x0010)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;				//使能按键所在的外部中断通道
	}
	else if (pin >= 0x0020 && pin <= 0x0200)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;				//使能按键所在的外部中断通道
	}
	else if (pin >= 0x0400 && pin <= 0x8000)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;				//使能按键所在的外部中断通道
	}
	NVIC_Init(&NVIC_InitStructure);  	  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
}

/*
 * 1号EC11左旋
 */
void E1_turn_left(void (*e1_left_function))
{
	e1_left_handler = e1_left_function;
}

/*
 * 1号EC11右旋
 */
void E1_turn_right(void (*e1_right_function))
{
	e1_right_handler = e1_right_function;
}


/*
 * 2号EC11左旋
 */
void E2_turn_left(void (*e2_left_function))
{
	e2_left_handler = e2_left_function;
}

/*
 * 2号EC11右旋
 */
void E2_turn_right(void (*e2_right_function))
{
	e2_right_handler = e2_right_function;
}
/*************************************************GPIO中断函数区域********************************************************/
void EXTI0_IRQHandler(void)
{
	delay_us(200);

	EXTI_ClearITPendingBit(EXTI_Line0);

}
void EXTI1_IRQHandler(void)
{
	delay_us(200);

	EXTI_ClearITPendingBit(EXTI_Line1);
}
void EXTI2_IRQHandler(void)
{
	delay_us(200);
	if (E2_2_check == LOW)
		e2_left_handler();

	if (E2_2_check == HIGH)
		e2_right_handler();

	EXTI_ClearITPendingBit(EXTI_Line2);
}

//void EXTI3_IRQHandler(void)
//{
//
//
//}

void EXTI9_5_IRQHandler(void)
{
	delay_us(200);

	if (E1_2_check == LOW)
		e1_left_handler();

	if (E1_2_check == HIGH)
		e1_right_handler();

	EXTI_ClearITPendingBit(EXTI_Line5);
}
void EXTI15_10_IRQHandler(void)
{
	delay_us(200);

	EXTI_ClearITPendingBit(EXTI_Line11);
}
