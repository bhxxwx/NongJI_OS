/*
 * Genlib.c
 *
 *  Created on: 2019年11月10日
 *      Author: WangXiang
 */
#include "GenLib.h"
#include "UserConfig.h"

char temp[usart_buffer_size] = { '\0' }; //串口输出暂存数组
uint16_t pwm_freq_B;
uint16_t pwm_freq_C;

uint8_t DB1, DB2, DB3, DB4, DC1, DC2, DC3, DC4;

typedef struct
{
	char RX_buffer[usart_buffer_size];
	uint16_t number;
	bool is_full;
} usart_cache; //串口缓冲区数据结构体

//初始化串口数据缓冲区
usart_cache Uart1_RX_Buffer;
usart_cache Uart2_RX_Buffer;

//static int uart1_in_cache = 0;
//static int uart2_in_cache = 0;

//检测是否已经初始化过相关功能
static bool adcinit_flag = false;
static bool pwminit_flag_B = false;
static bool pwminit_flag_C = false;

void (*function_handler)(void); //RTC定时秒中断中转函数指针
void (*systick_handler)(void); //中断中转函数指针

//各种初始化结构体
GPIO_InitTypeDef GPIO_InitStruct;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

/*
 * 中断优先级控制初始化
 * 只有抢占式优先级，没有响应优先级
 * 抢占优先级4位,可进行嵌套,可选值为:(高优先级)0~15(低优先级)
 * 使用串口2,3或中断的时候需要先初始化该函数
 */
void nvic_init()
{
	//只有抢占式优先级，没有响应优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}

/*
 * 串口1初始化
 * 优先级为1
 */
void usart_1_init(int bandrate)
{
	Uart1_RX_Buffer.is_full = false;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	USART_DeInit(USART1);
	nvic_init();

	//USART1_TX   PA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART1_RX	  PA.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Usart1 NVIC
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//USART
	USART_InitStructure.USART_BaudRate = bandrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//打开接收到数据中断
	USART_Cmd(USART1, ENABLE);
}

/*
 * 串口2控制初始化
 * bandrate:波特率设置
 * nvic_pree:抢占优先级4位,可进行嵌套,可选值为:(高优先级)0~15(低优先级)
 */
void usart_2_init(int bandrate, char nvic_pree)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	USART_DeInit(USART2);

	//USART2_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART2_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Usart2 NVIC
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = nvic_pree;	//抢占优先级
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//USART
	USART_InitStructure.USART_BaudRate = bandrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);	//打开接收到数据中断
	USART_Cmd(USART2, ENABLE);
}

/*
 * 串口3控制初始化
 * bandrate:波特率设置
 * nvic_pree:抢占优先级4位,可进行嵌套,可选值为:(高优先级)0~15(低优先级)
 */
void usart_3_init(int bandrate, char nvic_pree)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	USART_DeInit(USART3);

	//USART3_TX   PB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//USART3_RX	  PB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//Usart3 NVIC
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = nvic_pree;	//抢占优先级
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//USART
	USART_InitStructure.USART_BaudRate = bandrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);	//打开接收到数据中断
	USART_Cmd(USART3, ENABLE);
}

/*
 * 检测串口1数据缓冲区未读取数据个数
 * 返回值:	缓冲区数据个数(>=0)
 * 			-1->缓冲区以及溢出，存在数据丢失
 */
int check_usart1_cache()
{
	if (Uart1_RX_Buffer.is_full == true)
		return -1;
	else
		return Uart1_RX_Buffer.number;
}

/*
 * 检测串口2数据缓冲区未读取数据个数
 * 返回值:	缓冲区数据个数(>=0)
 * 			-1->缓冲区以及溢出，存在数据丢失
 */
int check_usart2_cache()
{
	if (Uart2_RX_Buffer.is_full == true)
		return -1;
	else
		return Uart2_RX_Buffer.number;
}

/*
 * 从串口1数据缓冲区读取全部数据,并清零缓冲区
 */
char *usart1_readToEnd()
{
	int i = 0;
	for (i = 0; i < Uart1_RX_Buffer.number; i++)	//将缓冲区数据复制到暂存数组
	{
		temp[i] = Uart1_RX_Buffer.RX_buffer[i];
	}
	for (; i < usart_buffer_size; i++)	//将暂存数组之后的数据清零
	{
		temp[i] = '\0';
	}
	Uart1_RX_Buffer.number = 0;	//清零数据位数,下一次数据接收放到缓冲区0位
	return temp;
}

/*
 * 从串口2数据缓冲区读取全部数据,并清零缓冲区
 */
char *usart2_readToEnd()
{
	int i = 0;
	for (i = 0; i < Uart2_RX_Buffer.number; i++)	//将缓冲区数据复制到暂存数组
	{
		temp[i] = Uart2_RX_Buffer.RX_buffer[i];
	}
	for (; i < usart_buffer_size; i++)	//将暂存数组之后的数据清零
	{
		temp[i] = '\0';
	}
	Uart2_RX_Buffer.number = 0;	//清零数据位数,下一次数据接收放到缓冲区0位
	return temp;
}

//void USART1_IRQHandler(void)
//{
//	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)	//接收到数据
//	{
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE); //清除接收中断标志
//
//		if (Uart1_RX_Buffer.number < usart_buffer_size) //如果当前数组小于缓冲区大小
//		{
//			Uart1_RX_Buffer.RX_buffer[Uart1_RX_Buffer.number] = USART_ReceiveData(USART1); //接收串口1数据到buff缓冲区
////			uart1_in_cache++;
//			Uart1_RX_Buffer.number++; //数据位数+1
//		} else
//		{
//			Uart1_RX_Buffer.is_full = true; //缓冲区已满
//		}
//	}
//}

void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收到数据
	{
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); //清除接收中断标志
		if (Uart2_RX_Buffer.number < usart_buffer_size)
		{
			Uart2_RX_Buffer.RX_buffer[Uart2_RX_Buffer.number] = USART_ReceiveData(USART2); //接收串口1数据到buff缓冲区
//			uart2_in_cache++;
			Uart2_RX_Buffer.number++;
		} else
		{
			Uart2_RX_Buffer.is_full = true;
		}
	}
}

//void USART3_IRQHandler(void)
//{
//	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //接收到数据
//	{
//		USART_ClearITPendingBit(USART3, USART_IT_RXNE); //清除接收中断标志
//		USART_ReceiveData(USART3);
//	}
//}

/*
 * 串口1发送数据
 * 参数:  data->需要发送的数据的char型指针
 * 		  length->发送的数据位数
 */
void usart_1_send(uint8_t *data, int length)
{
	int i = 0;
	for (; i < length; i++)
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
			//等待当前数据发送完成
			;
		USART_SendData(USART1, *(data + i));
	}
}

/*
 * 串口2发送数据
 * 参数:  data->需要发送的数据的char型指针
 * 		  length->发送的数据位数
 */
void usart_2_send(uint8_t *data, int length)
{
	int i = 0;
	for (; i < length; i++)
	{
		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
			//等待当前数据发送完成
			;
		USART_SendData(USART2, *(data + i));
	}
}

/*
 * 串口3发送数据
 * 参数:  data->需要发送的数据的char型指针
 * 		  length->发送的数据位数
 */
void usart_3_send(uint8_t *data, int length)
{
	int i = 0;
	for (; i < length; i++)
	{
		while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
			//等待当前数据发送完成
			;
		USART_SendData(USART3, *(data + i));
	}
}

/*
 * 设置CPU时钟为72MHz
 * 返回系统各时钟频率数值
 * 一般用不到
 */
RCC_ClocksTypeDef set_cpu_72MHz()
{
	RCC_ClocksTypeDef get_rcc_clock;
	//set the CPU clock as 72MHz
//	RCC_HSEConfig(RCC_HSE_OFF);
//	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
//	RCC_HSEConfig(RCC_HSE_ON);
//	RCC_PCLK1Config(RCC_HCLK_Div4); //APB1
//	RCC_WaitForHSEStartUp();
//	RCC_HCLKConfig(RCC_SYSCLK_Div1);
//	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	RCC_GetClocksFreq(&get_rcc_clock);
	return get_rcc_clock;
}

/* 定时器延迟,无中断,使用外部时钟信号
 * (精确)
 * 最高延时1.8s，即1864135us
 * 注意，使用操作系统时不能使用该函数
 */
void delay_tick_us(u32 time)
{
	u32 temp;
	SysTick->LOAD = 9 * time;
	SysTick->VAL = 0X00;   //清空计数器
	SysTick->CTRL = 0X01;   //使能，减到零是无动作，采用外部时钟源
	do
	{
		temp = SysTick->CTRL;   //读取当前倒计数值
	} while ((temp & 0x01) && (!(temp & (1 << 16)))); //等待时间到达
	SysTick->CTRL = 0x00; //关闭计数器
	SysTick->VAL = 0X00; //清空计数器
}

/* RTC计时器初始化
 * 设定优先级为1
 * 设定每一秒钟一次中断
 * 默认中断开启
 *
 * 使用 RTC_IT_ON; 开启RTC中断
 * 使用 RTC_IT_OFF; 关闭RTC中断
 */
void RTC_1s_it_init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	PWR_BackupAccessCmd(ENABLE);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);	//选择LSE时钟作为RTC时钟，此外还可以选择：LSI、HSE_Div128
	RCC_RTCCLKCmd(ENABLE);	//使能RTC时钟
	RTC_WaitForSynchro();
	RTC_WaitForLastTask();

	RTC_EnterConfigMode();	/// 允许配置
	RTC_SetPrescaler(62500);
	RTC_WaitForLastTask();
	RTC_ExitConfigMode(); //退出配置模式

	RTC_ClearITPendingBit(RTC_IT_SEC);
	RTC_WaitForSynchro();
	RTC_ITConfig(RTC_IT_SEC, ENABLE); //秒中断使能秒,用来产生一个可编程的周期性中断信号(最长可达1秒)。
	RTC_WaitForLastTask();

}

/* RTC中断函数传递函数
 * 参数:用户自定义函数的函数名(地址)
 * 例如:RTC_Handler(test_hander)
 */
void RTC_Handler(void (*temp_function))
{
	//RTC_ITConfig(RTC_IT_SEC, ENABLE); //秒中断使能秒,用来产生一个可编程的周期性中断信号(最长可达1秒)。
	function_handler = temp_function;
}

void RTC_IRQHandler(void) //RTC中断函数
{
	if (function_handler == 0x0)
	{
		RTC_ClearITPendingBit(RTC_IT_SEC); //清除中断标记
		return;
	}
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET) //秒中断
	{
		function_handler();
		RTC_ClearITPendingBit(RTC_IT_SEC); //清除中断标记
	}
}

/* 软件延时,系统时钟为72MHz时使用,否则不精确
 * 相差1us
 * (精确)
 */
void delay_us(__IO u32 time)
{
	int i = 0;
	unsigned char temp = 4;
	for (i = time; i >= 0; i--)
	{
		while (temp)
		{
			//__NOP();//单周期指令 1/72M=0.01389us
			temp--;
		}
		temp = 4;
	}
}

/*
 * 系统计时器中断，最大中断延时233ms
 * 参数:
 * 		u32 time:最小值为43,
 * 			最大值为72000000	,
 * 			特殊取值下:
 * 				1000 -> 1ms中断一次
 * 				100000 -> 10us中断一次
 * 				1000000 -> 1us中断一次
 * 		(*temp_function):中断服务函数地址
 *
 * 示例:systick_delay(1000,TEST_FUN);
 *
 * 注意!如果报错,请将src/stm32f1xx_it.c中的SysTick_Handler(void)函数注释掉
 */
void systick_delay(u32 time, void (*temp_function))
{
	u32 temp = SystemCoreClock / 10;
	temp = SystemCoreClock / time;
	if (temp > 0xFFFFFF)
		temp = 0xFFFFFF;
	if (SysTick_Config(temp))
	{
		/* Capture error */
		while (1)
			;
	}
	systick_handler = temp_function;
//	SysTick->CTRL = 0x00;   //失能SysTick
//	SysTick->VAL = 0x00;    //当前值清零
}

//#ifndef FREERTOS_CONFIG_H
//
//void SysTick_Handler(void)
//{
//	systick_handler();
//}
//#endif
/*
 * flash初始化
 * 擦除一页需22ms,且过程中CPU不能做任何事情
 * page:初始化页地址
 */
void Flash_Erase(uint32_t page)
{
	FLASH_SetLatency(FLASH_Latency_2); //FLASH时序延迟几个周期，等待总线同步操作
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //开启FLASH预读缓冲功能，加速FLASH的读取
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(page);
	FLASH_Lock();
}

/*
 * 以16位写入方式8位buffer
 * page:写入页地址
 * buffer:需要写入的8位数据指针
 * length:需要写入数据的长度
 * 该函数返回最后一个写入数据的flash地址，或者返回第几位数据写入失败
 */
uint32_t FlashWrite_8bitbuffer(uint32_t page, uint8_t *buffer, uint32_t length)
{
	uint16_t i, data = 0;
	uint32_t lastaddr = 0;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	for (i = 0; i < length; i += 2)
	{
		data = (*(buffer + i + 1) << 8) + (*(buffer + i));
		if (FLASH_ProgramHalfWord((uint32_t) (page + i), data) != FLASH_COMPLETE)
		{
			return (uint32_t) i;
		}
		lastaddr = (uint32_t) (page + i);
	}
	FLASH_Lock();
	return (uint32_t) lastaddr;
}

/*
 * 以16位写入方式16位buffer
 * page:写入页地址
 * buffer:需要写入的16位数据指针
 * length:需要写入数据的长度
 * 该函数返回最后一个写入数据的flash地址，或者返回第几位数据写入失败
 */
uint32_t FlashWrite_16bitbuffer(uint32_t page, uint16_t *buffer, uint32_t length)
{
	uint16_t i, data = 0;
	uint32_t lastaddr = 0;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	for (i = 0; i < length; i += 2)
	{
		data = *(buffer + i);
		if (FLASH_ProgramHalfWord((uint32_t) (page + i), data) != FLASH_COMPLETE)
		{
			return i;
		}
		lastaddr = (uint32_t) (page + i);
	}
	FLASH_Lock();
	return lastaddr;
}

/*
 * 连续8位读出数据
 * address:读取数据首地址
 * *buffer:读出数据存放的目标地址
 * length:需要读取数据个数
 * 返回当前数据地址
 */
uint32_t FlashRead_8bitbuffer(uint32_t address, uint8_t *buffer, uint32_t length)
{
	memcpy(buffer, (void *) address, length);
	return address + length - 1;
}

/*
 *	读出指定地址16位数据
 *	低地址在低位
 */
uint16_t FlashRead_16bitbuffer(uint32_t address)
{
	return *(__IO uint16_t*) address;
}

/*
 * 计时器初始化,最大计时1.8s,溢出没有任何提醒,注意使用场合,操作系统中不能使用
 * systick_on;//开始计时
 * systick_off;//停止计时
 */
void time_counter_init()
{
	SysTick->LOAD = 0xFFFFFF;
	SysTick->VAL = 0X00;   //清空计数器
	SysTick->CTRL = 0x00; //关闭计数器
}

/*
 * 返回计时值，单位us
 */
int time_counter_value()
{
	int finishtime = 0, time = 0;
	time = (SysTick->VAL);
	finishtime = (0xFFFFFF - time) / 9;
	return finishtime;
}

void adc_init()
{
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //使能ADC1通道时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE); //使能ADC2通道时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6,72M/6=12MHz,ADC最大不能超过14M
	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值
	ADC_DeInit(ADC2);  //复位ADC2,将外设 ADC2 的全部寄存器重设为缺省值
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器
	ADC_Init(ADC2, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器

	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	ADC_ResetCalibration(ADC1);	//使能复位校准
	while (ADC_GetResetCalibrationStatus(ADC1))
		;	//等待复位校准结束
	ADC_StartCalibration(ADC1);	 //开启AD校准
	while (ADC_GetCalibrationStatus(ADC1))
		;	 //等待校准结束

	ADC_Cmd(ADC2, ENABLE);	//使能指定的ADC2
	ADC_ResetCalibration(ADC2);	//使能复位校准
	while (ADC_GetResetCalibrationStatus(ADC2))
		;	//等待复位校准结束
	ADC_StartCalibration(ADC2);	 //开启AD校准
	while (ADC_GetCalibrationStatus(ADC2))
		;	 //等待校准结束
	adcinit_flag = true;
}

/*
 * GPIOA引脚模式设置
 * GPIO_Pin_:GPIO_Pin_X
 * GPIO_Mode: OUTPUT 推挽输出
 *			  INPUT 浮空输入
 *			  AIN 模拟输入(ADC)
 *			  IPU 上拉输入
 *			  IPD 下拉输入
 *			  AOT 模拟输出(DAC)
 */
void pinModeA(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	if (GPIO_Mode == GPIO_Mode_AIN)
	{
		if (adcinit_flag == false)
		{
			adc_init();
			/***************************************DAC******************************************************/
			DAC_InitTypeDef DAC_InitType;
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);	  //使能DAC通道时钟
			DAC_InitType.DAC_Trigger = DAC_Trigger_None;	//不使用触发功能 TEN1=0
			DAC_InitType.DAC_WaveGeneration = DAC_WaveGeneration_None;	//不使用波形发生
			DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;	//屏蔽、幅值设置
			DAC_InitType.DAC_OutputBuffer = DAC_OutputBuffer_Disable;	//DAC1输出缓存关闭 BOFF1=1
			DAC_Init(DAC_Channel_1, &DAC_InitType);	 //初始化DAC通道1
			DAC_Init(DAC_Channel_2, &DAC_InitType);	 //初始化DAC通道2

			DAC_Cmd(DAC_Channel_1, ENABLE);  //使能DAC通道1
			DAC_Cmd(DAC_Channel_2, ENABLE);  //使能DAC通道2

			DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12位右对齐数据格式设置DAC值
			DAC_SetChannel2Data(DAC_Align_12b_R, 0);  //12位右对齐数据格式设置DAC值
		}
	}
}

/*
 * GPIOB引脚模式设置
 * GPIO_Pin_:GPIO_Pin_X
 * GPIO_Mode: OUTPUT 推挽输出
 *			  INPUT 浮空输入
 *			  IPU 上拉输入
 *			  IPD 下拉输入
 *			  PWM PWM输出,默认频率10kHz
 */
void pinModeB(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	if (GPIO_Mode == GPIO_Mode_AF_PP)
	{
		if (pwminit_flag_B == false)
		{
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

			//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
			//PWM频率:72000/(899+1)=80khz
			TIM_TimeBaseStructure.TIM_Period = PWM_Freq_B;
			pwm_freq_B = PWM_Freq_B;

			TIM_TimeBaseStructure.TIM_Prescaler = 0; //设置用来作为TIMx时钟频率除数的预分频值
			TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
			TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
			TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
			TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高
			TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
			TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

#ifdef PWM_Complementary_B
			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
#endif
			TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
			TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

			TIM_CtrlPWMOutputs(TIM4, ENABLE);	//MOE 主输出使能

			TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH1预装载使能
			TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH2预装载使能
			TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH3预装载使能
			TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //CH4预装载使能

			TIM_ARRPreloadConfig(TIM4, ENABLE); //使能TIMx在ARR上的预装载寄存器

			TIM_Cmd(TIM4, ENABLE);  //使能TIM1
			pwminit_flag_B = true;
		}
	}
}

/*
 * GPIOC引脚模式设置
 * GPIO_Pin_:GPIO_Pin_X
 * GPIO_Mode: OUTPUT 推挽输出
 *			  INPUT 浮空输入
 *			  AIN 模拟输入
 *			  IPU 上拉输入
 *			  IPD 下拉输入
 *			  PWM PWM输出,默认频率100kHz
 */
void pinModeC(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	if (GPIO_Mode == GPIO_Mode_AIN)
	{
		if (adcinit_flag == false)
		{
			adc_init();
		}
	}
	if (GPIO_Mode == GPIO_Mode_AF_PP)
	{
		if (pwminit_flag_C == false)
		{
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

			//设置在下一个更新事件装入活动的自动重装载寄存器周期的值
			//PWM频率:72000/(899+1)=80khz
			TIM_TimeBaseStructure.TIM_Period = PWM_Freq_C;
			pwm_freq_C = PWM_Freq_C;

			TIM_TimeBaseStructure.TIM_Prescaler = 0; //设置用来作为TIMx时钟频率除数的预分频值
			TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
			TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

			TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
			TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
			TIM_OCInitStructure.TIM_Pulse = 0; //设置待装入捕获比较寄存器的脉冲值
			TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性高
			TIM_OC1Init(TIM8, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
			TIM_OC2Init(TIM8, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
#ifdef PWM_Complementary_C
			        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
#endif
			TIM_OC3Init(TIM8, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx
			TIM_OC4Init(TIM8, &TIM_OCInitStructure);  //根据TIM_OCInitStruct中指定的参数初始化外设TIMx

			TIM_CtrlPWMOutputs(TIM8, ENABLE);	//MOE 主输出使能

			TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH1预装载使能
			TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH2预装载使能
			TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH3预装载使能
			TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);  //CH4预装载使能

			TIM_ARRPreloadConfig(TIM8, ENABLE); //使能TIMx在ARR上的预装载寄存器

			TIM_Cmd(TIM8, ENABLE);  //使能TIM1
			pwminit_flag_C = true;
		}
	}
}

/*
 * GPIOD引脚模式设置
 * GPIO_Pin_:GPIO_Pin_X
 * GPIO_Mode: OUTPUT 推挽输出
 *			  INPUT  浮空输入
 *			  IPU    上拉输入
 *			  IPD    下拉输入
 */
void pinModeD(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/*
 * 读取GPIOA端口 Pin_0 ~ Pin_7的模拟输入
 * 使用前请将A端口模式置为模拟输入模式，例:
 * 		pinModeA(GPIO_Pin_7,AIN);
 */
uint16_t analogeReadA(uint16_t GPIO_Pin)
{
	uint8_t ch = 0;
	switch (GPIO_Pin)
	{
		case GPIO_Pin_0:
			ch = 0;
			break;
		case GPIO_Pin_1:
			ch = 1;
			break;
		case GPIO_Pin_2:
			ch = 2;
			break;
		case GPIO_Pin_3:
			ch = 3;
			break;
		case GPIO_Pin_4:
			ch = 4;
			break;
		case GPIO_Pin_5:
			ch = 5;
			break;
		case GPIO_Pin_6:
			ch = 6;
			break;
		case GPIO_Pin_7:
			ch = 7;
			break;
		default:
			ch = 17;
	}
	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5);	//ADC1,ADC通道,采样时间为239.5周期

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	//使能指定的ADC1的软件转换启动功能

	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
		;		//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

/*
 * 读取GPIOC端口 Pin_0 ~ Pin_5的模拟输入
 * 使用前请将C端口模式置为模拟输入模式，例:
 * 		pinModeC(GPIO_Pin_7,AIN);
 */
uint16_t analogeReadC(uint16_t GPIO_Pin)
{
	uint8_t ch = 0;
	switch (GPIO_Pin)
	{
		case GPIO_Pin_0:
			ch = 10;
			break;
		case GPIO_Pin_1:
			ch = 11;
			break;
		case GPIO_Pin_2:
			ch = 12;
			break;
		case GPIO_Pin_3:
			ch = 13;
			break;
		case GPIO_Pin_4:
			ch = 14;
			break;
		case GPIO_Pin_5:
			ch = 15;
			break;
		default:
			ch = 17;
	}
	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC2, ch, 1, ADC_SampleTime_239Cycles5);	//ADC2,ADC通道,采样时间为239.5周期

	ADC_SoftwareStartConvCmd(ADC2, ENABLE);	//使能指定的ADC2的软件转换启动功能

	while (!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC))
		;		//等待转换结束

	return ADC_GetConversionValue(ADC2);	//返回最近一次ADC2规则组的转换结果
}

/*
 * DAC模拟输出
 * 		GPIO_Pin  引脚控制,值为GPIO_Pin_4或5
 * 		value	输出电压控制(0~3.3),值为0~4095
 */
void analogWrite(uint16_t GPIO_Pin, uint16_t value)
{
	if (GPIO_Pin == GPIO_Pin_4)
		DAC_SetChannel1Data(DAC_Align_12b_R, value);  //12位右对齐数据格式设置DAC值
	if (GPIO_Pin == GPIO_Pin_5)
		DAC_SetChannel2Data(DAC_Align_12b_R, value);  //12位右对齐数据格式设置DAC值
}

/*
 * GPIOB口的PWM占空比调节函数
 * 如果开启PWM_Complementary则没办法设置PB8和PB9的占空比,此时PB8和PB9的占空比分别由PB6和PB7决定
 * 参数:
 * 		GPIO_Pin 调节的PB端口引脚
 * 		Duty 调节的占空比值(0~100)
 */
void DutyCycleB(uint16_t GPIO_Pin, uint8_t Duty)
{
	int16_t dutycycle = pwm_freq_B * Duty * 0.01;

	if (GPIO_Pin == GPIO_Pin_6)
	{
		DB1 = Duty;
		TIM_SetCompare1(TIM4, dutycycle);
#ifdef PWM_Complementary_B
		DB3=Duty;
		TIM_SetCompare3(TIM4, dutycycle);
#endif
	}
	if (GPIO_Pin == GPIO_Pin_7)
	{
		DB2 = Duty;
		TIM_SetCompare2(TIM4, dutycycle);
#ifdef PWM_Complementary_B
		DB4=Duty;
		TIM_SetCompare4(TIM4, dutycycle);
#endif
	}
#ifndef PWM_Complementary_B
	if (GPIO_Pin == GPIO_Pin_8)
	{
		DB3 = Duty;
		TIM_SetCompare3(TIM4, dutycycle);
	}
	if (GPIO_Pin == GPIO_Pin_9)
	{
		DB4 = Duty;
		TIM_SetCompare4(TIM4, dutycycle);
	}
#endif

}

/*
 * GPIOC口的PWM占空比调节函数
 * 参数:
 * 		GPIO_Pin 调节的PB端口引脚
 * 		Duty 调节的占空比值(0~100)
 */
void DutyCycleC(uint16_t GPIO_Pin, uint8_t Duty)
{
	int16_t dutycycle = pwm_freq_C * Duty * 0.01;

	if (GPIO_Pin == GPIO_Pin_6)
	{
		DC1 = Duty;
		TIM_SetCompare1(TIM8, dutycycle);
#ifdef PWM_Complementary_C
		DC3=Duty;
		TIM_SetCompare3(TIM8, dutycycle);
#endif
	}
	if (GPIO_Pin == GPIO_Pin_7)
	{
		DC2 = Duty;
		TIM_SetCompare2(TIM8, dutycycle);
#ifdef PWM_Complementary_C
		DC4=Duty;
		TIM_SetCompare4(TIM8, dutycycle);
#endif
	}
#ifndef PWM_Complementary_C
	if (GPIO_Pin == GPIO_Pin_8)
	{
		DC3 = Duty;
		TIM_SetCompare3(TIM8, dutycycle);
	}
	if (GPIO_Pin == GPIO_Pin_9)
	{
		DC4 = Duty;
		TIM_SetCompare4(TIM8, dutycycle);
	}
#endif
}

/*
 *  设置PWM频率
 *  参数:
 *  	GPIOx 值为GPIOB或者GPIOC,设置PB口或者PC口PWM的频率
 *  	freq 输入PWM频率,范围:1010~72000000Hz
 */
void SetPWMfreq(GPIO_TypeDef* GPIOx, int freq)
{
	uint16_t arr = 72000000 / (freq) + 1;
	if (arr > 65535)
		arr = 65535;
	if (GPIOx == GPIOB)
	{
		TIM_TimeBaseStructure.TIM_Period = arr;
		pwm_freq_B = arr;
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
		TIM_SetCompare1(TIM4, pwm_freq_B * DB1 * 0.01);
		TIM_SetCompare2(TIM4, pwm_freq_B * DB2 * 0.01);
		TIM_SetCompare3(TIM4, pwm_freq_B * DB3 * 0.01);
		TIM_SetCompare4(TIM4, pwm_freq_B * DB4 * 0.01);
	}
	if (GPIOx == GPIOC)
	{
		TIM_TimeBaseStructure.TIM_Period = arr;
		pwm_freq_C = arr;
		TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
		TIM_SetCompare1(TIM8, pwm_freq_C * DC1 * 0.01);
		TIM_SetCompare2(TIM8, pwm_freq_C * DC2 * 0.01);
		TIM_SetCompare3(TIM8, pwm_freq_C * DC3 * 0.01);
		TIM_SetCompare4(TIM8, pwm_freq_C * DC4 * 0.01);
	}
}

#ifdef STM32F10X_HD
/*
 * 计时器中断!!!不准，还没有测试
 * 时钟频率为2kHz,最小时基为0.5ms,最大计时时间为32s
 * 参数:
 * 		times 计时时间,范围(1~32000)单位:毫秒
 * 	中断函数在BSP_Tim.c文件中
 */
void TIM7_init(uint16_t times)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //配置抢断优先级
	NVIC_Init(&NVIC_InitStructure); //设置存入寄存器

	if (times <= 0)
		times = 1000;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	//预分频系数为36000-1,这样计数器时钟为72MHz/36000 = 2kHz,分辨率为0.5ms
	TIM_TimeBaseStructure.TIM_Prescaler = 36000 - 1;
	//设置时钟分割 TIM_CKD_DIV1=0x0000,不分割
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	//设置计数器模式为向上计数模式
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//设置计数溢出大小，每计2000个数就产生一个更新事件
	TIM_TimeBaseStructure.TIM_Period = times * 2 - 1;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0; //将配置应用到TIM7中
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
	TIM_UpdateRequestConfig( TIM7, TIM_UpdateSource_Regular); //使能计数器
	TIM_Cmd(TIM7, ENABLE); //使能中断
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE); //清除标志位
	TIM_ClearFlag(TIM7, TIM_FLAG_Update);
}
#endif

//void TIM7_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM7, TIM_IT_Update) == SET)
//	{
//		delay_us(100);
//		TIM_ClearFlag(TIM7, TIM_FLAG_Update);
//	}
//}

/***************************************************CAN总线相关操作*******************************************************/
/*
 * CAN总线基本设置
 * 引脚,中断,时钟设置
 */
void CAN_INIT()
{
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,
	        ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

#ifdef USE_CAN_PB
	//重映射到PB端口
	/* Configure CAN pin: RX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Configure CAN pin: TX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_PinRemapConfig(GPIO_Remap1_CAN1, ENABLE);	   //重影射CAN IO脚到 PB8,PB9
#endif

#ifdef USE_CAN_PA
	//不重新映射,PA端口
	/* Configure CAN pin: RX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure CAN pin: TX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif

	NVIC_InitTypeDef NVIC_InitStructure;
	/* Configure the NVIC Preemption Priority Bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	/* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
	/* enabling interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
 * CAN总线基本设置(轮询模式,不使用中断)
 * 设置总线模式,总线频率(目前250kHz),过滤器
 */
void CAN_POLLING_INIT()
{
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
	CAN_DeInit(CAN1);
//	CAN_OperatingModeRequest(CAN1, CAN_OperatingMode_Initialization);
	/* CAN register init */
	CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = 0;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	CAN_InitStructure.CAN_Prescaler = 12;	   //  36M/(1+8+3)/12=250k
	CAN_Init(CAN1, &CAN_InitStructure);
//	CAN_OperatingModeRequest(CAN1, CAN_OperatingMode_Normal);
	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 1;
	CAN_FilterInitStructure.CAN_FilterActivation = DISABLE;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

}

/*
 * 轮询模式消息发送
 */
void CAN_POLLING_SEND(CanTxMsg TxMessage)
{
	uint8_t TransmitMailbox = 0;
	TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);
	int i = 0;
	while ((CAN_TransmitStatus(CAN1, TransmitMailbox) != CANTXOK) && (i != 0xFF))
	{
		i++;
	}
}

/*
 * 轮询模式消息接收
 * 返回CanRxMsg类型数据
 */
CanRxMsg CAN_POLLING_REC()
{
	int i = 0;
	while ((CAN_MessagePending(CAN1, CAN_FIFO0) < 1) && (i != 0xFF))
	{
		i++;
	}
	/* receive */
	CanRxMsg RxMessage;
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
	return RxMessage;
}

/*
 * CAN总线中断方式初始化
 * 设置总线模式,总线频率(目前250kHz)
 */
void CAN_IT_INIT()
{
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;

	/* CAN register init */
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);

	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_8tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_3tq;
	CAN_InitStructure.CAN_Prescaler = 12;   //  36M/(1+8+3)/12=250k
	CAN_Init(CAN1, &CAN_InitStructure);

	/* CAN filter init */
	CAN_FilterInitStructure.CAN_FilterNumber = 1;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);

	/* CAN FIFO0 message pending interrupt enable */
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);
}

/*
 * 中断模式数据发送
 */
void CAN_IT_SEND(CanTxMsg TxMessage)
{
	CAN_Transmit(CAN1, &TxMessage);
}

//CanRxMsg CAN_IT_REC();

/*
 * CAN/USB总线接收中断
 */
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

	USB_Istr();

}

/*
 * 看门狗初始化,并且使能
 * 参数:
 * 		Div:分频系数选择,可为以下值:
  *     	@arg IWDG_Prescaler_4: IWDG prescaler set to 4
  *     	@arg IWDG_Prescaler_8: IWDG prescaler set to 8
  *     	@arg IWDG_Prescaler_16: IWDG prescaler set to 16
  *     	@arg IWDG_Prescaler_32: IWDG prescaler set to 32
  *     	@arg IWDG_Prescaler_64: IWDG prescaler set to 64
  *     	@arg IWDG_Prescaler_128: IWDG prescaler set to 128
  *     	@arg IWDG_Prescaler_256: IWDG prescaler set to 256
 *
 *      No:计数数值
 *
 *      看门狗时钟频率为40kHz,最终计时时间为(1/40000)*Div*No (s)
 *      例如64分频,重装载值为625,那么看门狗计时为1秒钟
 *
 */
void IWDG_INIT(uint8_t Div,u16 No)
{
 	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //使能对寄存器IWDG_PR和IWDG_RLR的写操作

	IWDG_SetPrescaler(Div);  //设置IWDG预分频值

	IWDG_SetReload(No);  //设置IWDG重装载值

	IWDG_ReloadCounter();  //按照IWDG重装载寄存器的值重装载IWDG计数器

	IWDG_Enable();  //使能IWDG
}

/*喂狗*/
void IWDG_Feed(void)
{
 	IWDG_ReloadCounter();
}
