/*
********************************************************************************
*
*                                 bsp.h
*
* File          : bsp.h
* Version       : V1.0
* Author        : whq
* Mode          : Thumb2
* Toolchain     : 
* Description   : BSP硬件初始化
*                
* History       :
* Date          : 2013.07.21
*******************************************************************************/


#ifndef _BSP_H_
#define _BSP_H_


#include <stdint.h>
#include <stddef.h>

/******************************系统配置****************************************/

typedef enum {
    eLED_0,
    eLED_1,
    eLED_2,
    
    eGPIO_MAX_COUNT,
}GPIO_ENUM_t;



/******************************宏定义******************************************/

//LED关
#define LED_OFF(a)              BSP_GpioSet(a)
//LED开
#define LED_ON(a)               BSP_GpioReset(a)


//IO置高
#define IO_SET(a)               BSP_GpioSet(a)
//IO置低
#define IO_RESET(a)             BSP_GpioReset(a)
//读IO口上的电平
#define IO_READ(a)              BSP_GpioRead(a)
//IO电平翻转
#define IO_TOGGLE(a)            BSP_GpioToggle(a)


#define BSP_CONEOF(a)           ((sizeof(a)) / (sizeof(*a)))



/******************************************************************************/


void IWDG_Init(void);
void SysTickInit(void);
void WDG_Clear(void);


void BSP_PortWrite(int32_t n, int32_t value);
void BSP_SystemReset(void);



#endif
/******************************END*********************************************/

