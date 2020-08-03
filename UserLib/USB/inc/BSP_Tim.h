/*
********************************************************************************
*
*                                 BSP_Tim.h
*
* File          : BSP_Tim.h
* Version       : V1.0
* Author        : whq
* Mode          : Thumb2
* Toolchain     : 
* Description   : 定时器 驱动程序 头文件
*                
* History       :
* Date          : 2013.07.25
*******************************************************************************/


#ifndef _BSP_TIM_H_
#define _BSP_TIM_H_

#include <stdint.h>

#define TIM_1_EN                1
#define TIM_2_EN                1
#define TIM_3_EN                1
#define TIM_4_EN                1
#define TIM_5_EN                1
#define TIM_6_EN                1
#define TIM_7_EN                1
#define TIM_8_EN                1


#define TIM_IRQEnter()                  //进中断执行函数
#define TIM_IRQExit()                   //出中断执行函数

typedef enum {
#if TIM_1_EN
    TIM_1,
#endif
#if TIM_2_EN
    TIM_2,
#endif
#if TIM_3_EN
    TIM_3,
#endif
#if TIM_4_EN
    TIM_4,
#endif
#if TIM_5_EN
    TIM_5,
#endif
#if TIM_6_EN
    TIM_6,
#endif
#if TIM_7_EN
    TIM_7,
#endif
#if TIM_8_EN
    TIM_8,
#endif
    TIM_MAX
}TIM_ENUM;


#define TIMn                    TIM_MAX


/**
 * @brief Definition for Tim1
 */ 
#define BSP_TIM1                            TIM1
#define BSP_TIM1_CLK                        RCC_APB2Periph_TIM1
#define BSP_TIM1_IRQn                       TIM1_UP_IRQn

/**
 * @brief Definition for Tim2
 */ 
#define BSP_TIM2                            TIM2
#define BSP_TIM2_CLK                        RCC_APB1Periph_TIM2
#define BSP_TIM2_IRQn                       TIM2_IRQn

/**
 * @brief Definition for Tim3
 */ 
#define BSP_TIM3                            TIM3
#define BSP_TIM3_CLK                        RCC_APB1Periph_TIM3
#define BSP_TIM3_IRQn                       TIM3_IRQn

/**
 * @brief Definition for Tim4
 */ 
#define BSP_TIM4                            TIM4
#define BSP_TIM4_CLK                        RCC_APB1Periph_TIM4
#define BSP_TIM4_IRQn                       TIM4_IRQn

/**
 * @brief Definition for Tim5
 */ 
#define BSP_TIM5                            TIM5
#define BSP_TIM5_CLK                        RCC_APB1Periph_TIM5
#define BSP_TIM5_IRQn                       TIM5_IRQn

/**
 * @brief Definition for Tim6
 */ 
#define BSP_TIM6                            TIM6
#define BSP_TIM6_CLK                        RCC_APB1Periph_TIM6
#define BSP_TIM6_IRQn                       TIM6_IRQn

/**
 * @brief Definition for Tim7
 */ 
#define BSP_TIM7                            TIM7
#define BSP_TIM7_CLK                        RCC_APB1Periph_TIM7
#define BSP_TIM7_IRQn                       TIM7_IRQn

/**
 * @brief Definition for Tim8
 */ 
#define BSP_TIM8                            TIM8
#define BSP_TIM8_CLK                        RCC_APB2Periph_TIM8
#define BSP_TIM8_IRQn                       TIM8_UP_IRQn


/******************************函数声明****************************************/
void BSP_TimOpen(uint8_t TIM_x, uint32_t prescaler, uint32_t autoreload, void(*CallBack)(void));
void BSP_TimClose(uint8_t TIM_x);
void BSP_TimITStateSet(uint8_t TIM_x, uint8_t state);
void BSP_TimIRQCallbackSet(uint8_t TIM_x, void(*CallBack)(void));
void BSP_TimAutoreloadSet(uint8_t TIM_x, uint32_t autoreload);
void BSP_TimCounterSet(uint8_t TIM_x, uint32_t Counter);
uint32_t BSP_TimCounterGet(uint8_t TIM_x);
uint32_t BSP_TimClockGet(void);




#endif


