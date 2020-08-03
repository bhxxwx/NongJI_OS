/*
 * UserConfig.h
 *
 *  Created on: 2020年4月12日
 *      Author: WangXiang
 */

#ifndef USERCONFIG_H_
#define USERCONFIG_H_
#include <stddef.h>
#include <string.h>
#include <misc.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "stm32f10x.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_wwdg.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_iwdg.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_dbgmcu.h"

#include "Gen/GenLib.h"
#include "oled/oled.h"
#include "keys/keys.h"

#include "SPI_FLASH/SPI_FLASH.h"

#include "USB/inc/BSP.h"
#include "USB/inc/BSP_tim.h"
#include "USB/inc/HW_Config.h"
#include "USB/inc/usb_istr.h"
#include "USB/inc/usb_printf.h"
/*---------------------------OLED-IIC设置--------------------------*/
//如果选用了B端口即修改为 IIC_SDA_Poart_B即可
#define IIC_SDA_Poart_A

//如果选用了其他引脚如8号引脚即 GPIO_Pin_8即可
#define IIC_SDA_Pin GPIO_Pin_7

#define IIC_SCL_Poart_A
#define IIC_SCL_Pin GPIO_Pin_6

/*------------------------串口缓冲区大小设置------------------------*/
#define usart_buffer_size 500

/*-----------------------------PWM设置-----------------------------*/
//#define PWM_Complementary_B//定义PB口PWM存在互补输出,取消注释后PB6于PB8互补,PB7于PB9互补
//#define PWM_Complementary_C//定义PC口PWM存在互补输出,取消注释后PC6于PC8互补,PC7于PC9互补

/*
 * PWM默认频率设置
 * PWM频率计算公式:72000/(899+1)=80kHz
 */
#define PWM_Freq_B 7199 //10KHz
//#define PWM_Freq_B 1799//40kHz
//#define PWM_Freq_B 899//80kHz
//#define PWM_Freq_B 719//100kHz

#define PWM_Freq_C 719 //100KHz

/*------------------------CAN总线设置------------------------------*/
#define USE_CAN_PA //使用PA口CAN总线<不重映射管脚>
//#define USE_CAN_PB	//使用PB口CAN总线<重映射管脚>

/*------------------------编码开关引脚设置-------------------------*/
//若使用还需要到keys/keys.c里面改写中断服务函数!!
#define E1_1_Poart_B
#define E1_1 GPIO_Pin_1
#define E1_1_IT GPIO_PinSource1

#define E1_2_Poart_B
#define E1_2 GPIO_Pin_0
#define E1_2_IT GPIO_PinSource0

#define E1_3_Poart_C
#define E1_3 GPIO_Pin_5
#define E1_3_IT GPIO_PinSource5

#define E2_1_Poart_B
#define E2_1 GPIO_Pin_11
#define E2_1_IT GPIO_PinSource11

#define E2_2_Poart_B
#define E2_2 GPIO_Pin_10
#define E2_2_IT GPIO_PinSource10

#define E2_3_Poart_B
#define E2_3 GPIO_Pin_2
#define E2_3_IT GPIO_PinSource2

#endif /* USERCONFIG_H_ */
