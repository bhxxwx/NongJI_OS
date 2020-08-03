/*
 * GenLib.h
 *
 *  Created on: 2019年11月10日
 *      Author: WangXiang
 *  LastWrite:2020/4/19
 *  Version:V2.0
 */

#ifndef GENLIB_H_
#define GENLIB_H_

#include "UserConfig.h"

/***************************************************宏定义区域**********************************************************/
//Flash 页面宏定义
#define page250 0x0807D000
#define page251 0x0807D800
#define page252 0x0807E000
#define page253 0x0807E800
#define page254 0x0807F000
#define page255 0x0807F800

//计时器启动/关闭
#define systick_on SysTick->CTRL = 0X01
#define systick_off SysTick->CTRL = 0x00

//GPIO模式宏定义
#define OUTPUT GPIO_Mode_Out_PP
#define INPUT GPIO_Mode_IN_FLOATING
#define AIN GPIO_Mode_AIN
#define IPU GPIO_Mode_IPU
#define IPD GPIO_Mode_IPD
#define PWM GPIO_Mode_AF_PP
#define AOT GPIO_Mode_AIN
#define HIGH 0x1
#define LOW 0x0

//RTC定时开/关
#define RTC_IT_ON 	RTC_WaitForSynchro(),RTC_ITConfig(RTC_IT_SEC, ENABLE),RTC_WaitForLastTask()//开启RTC每秒中断
#define RTC_IT_OFF 	RTC_WaitForSynchro(),RTC_ITConfig(RTC_IT_SEC, DISABLE),RTC_WaitForLastTask()//关闭RTC每秒中断

/***************************************************函数区域************************************************************/
//中断初始化->组4->只有抢占优先级->0~15
void nvic_init();

/***************************************************串口 相关操作********************************************************/
//串口初始化
void usart_1_init(int bandrate);
void usart_2_init(int bandrate, char nvic_pree);
void usart_3_init(int bandrate, char nvic_pree);

//串口缓冲区检查
int check_usart1_cache();
int check_usart2_cache();

//读取串口缓冲区全部数据,并清零缓冲区
char *usart1_readToEnd();
char *usart2_readToEnd();

//串口发送数据,串口1使用printf函数故一般不用usart_1_send()
void usart_1_send(uint8_t *data, int length);
void usart_2_send(uint8_t *data, int length);
void usart_3_send(uint8_t *data, int length);

/***************************************************时钟频率 相关操作****************************************************/
//设置CPU时钟为72MHz,返回系统各时钟频率数值
RCC_ClocksTypeDef set_cpu_72MHz();

/***************************************************定时 相关操作********************************************************/
void delay_us(__IO u32 time); //软件延时
void delay_tick_us(u32 time); //硬件延时,使用操作系统时不可使用

/***************************************************计时 相关操作********************************************************/
void RTC_1s_it_init(); //RTC计时1s初始化
void RTC_Handler(void (*temp_function)); //RTC中断函数传递函数

//系统计时器SystemTick,使用操作系统时不可使用
void systick_delay(u32 time, void (*temp_function)); //系统计时器SystemTick定时中断

void time_counter_init(); //计时器初始化
int time_counter_value(); //系统计时器返回计时值

void TIM7_init(uint16_t times);

/***************************************************GPIO 相关操作********************************************************/
//GPIO模式设定
void pinModeA(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode);
void pinModeB(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode);
void pinModeC(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode);
void pinModeD(uint16_t GPIO_Pin, GPIOMode_TypeDef GPIO_Mode);

//GPIO引脚高低电平设定
#define digitalWriteA(GPIO_Pin, GPIO_Mode) GPIO_WriteBit(GPIOA, GPIO_Pin, GPIO_Mode)
#define digitalWriteB(GPIO_Pin, GPIO_Mode) GPIO_WriteBit(GPIOB, GPIO_Pin, GPIO_Mode)
#define digitalWriteC(GPIO_Pin, GPIO_Mode) GPIO_WriteBit(GPIOC, GPIO_Pin, GPIO_Mode)
#define digitalWriteD(GPIO_Pin, GPIO_Mode) GPIO_WriteBit(GPIOD, GPIO_Pin, GPIO_Mode)

//读取GPIO引脚电平
#define digitalReadA(GPIO_Pin) GPIO_ReadInputDataBit(GPIOA, GPIO_Pin)
#define digitalReadB(GPIO_Pin) GPIO_ReadInputDataBit(GPIOB, GPIO_Pin)
#define digitalReadC(GPIO_Pin) GPIO_ReadInputDataBit(GPIOC, GPIO_Pin)
#define digitalReadD(GPIO_Pin) GPIO_ReadInputDataBit(GPIOD, GPIO_Pin)

//读取ADC
uint16_t analogeReadA(uint16_t GPIO_Pin);
uint16_t analogeReadC(uint16_t GPIO_Pin);

//写DAC
void analogWrite(uint16_t GPIO_Pin, uint16_t value);

//设置PWM占空比
void DutyCycleB(uint16_t GPIO_Pin, uint8_t Duty);
void DutyCycleC(uint16_t GPIO_Pin, uint8_t Duty);

void SetPWMfreq(GPIO_TypeDef* GPIOx, int freq);

void adc_init();

/***************************************************CAN总线相关操作*******************************************************/
void CAN_INIT();

void CAN_POLLING_INIT();
void CAN_POLLING_SEND(CanTxMsg TxMessage);
CanRxMsg CAN_POLLING_REC();

void CAN_IT_INIT();
void CAN_IT_SEND(CanTxMsg TxMessage);
//CanRxMsg CAN_IT_REC();

/***************************************************看门狗相关操作*******************************************************/
void IWDG_INIT(uint8_t Div,u16 No);
void IWDG_Feed(void);

/***************************************************内置Flash相关操作********************************************************/
/*
 * FLASH使用注意事项
 *	FLASH只接受16位写入方式，第一次写地址为0x0807F800，那下一次为0x0807F802，
 *	FLASH中，地址每加一代表1个字节8位，
 *	FLASH中，所谓的写操作都是按位与，
 */
/*
 * 例:
 *   addr = FlashWrite_8bitbuffer(page254, (uint8_t*) "hello", 5); //向page254写入5个8位数据"hello",并得到最后一个数据的flash地址
 *
 *   //FLASH只接受16位写入方式，因此下次写入需要将addr+2
 *   FlashWrite_16bitbuffer(addr + 2, &temp, 1); //向addr下一个地址写入0x1234的16位数据
 *
 *   addr = FlashRead_8bitbuffer(page254, clean, 3); //连续读出3个8位数据,返回最后一个读出数据的地址(uint8_t clean[6] = { 0x00 };)
 */
void Flash_Erase(uint32_t page);
uint32_t FlashWrite_8bitbuffer(uint32_t page, uint8_t *buffer, uint32_t length);
uint32_t FlashWrite_16bitbuffer(uint32_t page, uint16_t *buffer, uint32_t length);
uint32_t FlashRead_8bitbuffer(uint32_t address, uint8_t *buffer, uint32_t length);
uint16_t FlashRead_16bitbuffer(uint32_t address);

#endif /* GENLIB_H_ */
