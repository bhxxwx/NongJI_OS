/*
********************************************************************************
*
*                                 bsp.c
*
* File          : bsp.c
* Version       : V1.0
* Author        : whq
* Mode          : Thumb2
* Toolchain     : 
* Description   :   系统初始化
                    IO初始化
                    时钟初始化
*                
* History       :
* Date          : 2013.07.21
*******************************************************************************/



#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_iwdg.h"
#include "USB/inc/BSP.h"


/*******************************************************************************
函 数 名：void IWDG_Init(void)
功能描述：看门狗初始化
入口参数：
返回参数：
创建时间: 2011.6.24
********************************************************************************/
void IWDG_Init(void)
{
    IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
    IWDG_SetPrescaler( IWDG_Prescaler_128);	//最小
    IWDG_SetReload(0x138 * 5);		//40KHz内部时钟 (1/40000 * 128 * 0x138 * 5= 5s )
    IWDG_WriteAccessCmd( IWDG_WriteAccess_Disable );
    IWDG_Enable();
    IWDG_ReloadCounter();
}

/*******************************************************************************
* Function Name : void IWDG_Clear(void)
* Description   : 清看门狗
* Input         : 
* Output        : 
* Other         : 
* Date          : 2014.04.13
*******************************************************************************/
void WDG_Clear(void)
{
    IWDG_ReloadCounter();
}

/*******************************************************************************
* Function Name :void SysTickInit(void)
* Description   :系统定时器时间配置
* Input         :
* Output        :
* Other         :时基为1ms
* Date          :2011.11.03  12:59:13
*******************************************************************************/
void SysTickInit(void)
{
    RCC_ClocksTypeDef rccClk = {0};
    
    RCC_GetClocksFreq(&rccClk);
    
    SysTick_Config(rccClk.HCLK_Frequency / 1000);			//uCOS时基1ms
}

/*******************************************************************************
* Function Name :void InterruptOrder(void)
* Description   :中断向量，优先级
* Input         :
* Output        :
* Other         :
* Date          :2011.10.27  11:50:05
*******************************************************************************/
void NVIC_Configuration(void)
{
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4);//优先级设置  全为抢占式优先级
}

/*******************************************************************************
* Function Name : void BSP_SystemReset(void)
* Description   : 系统复位
* Input         : 
* Output        : 
* Other         : 
* Date          : 2014.08.25
*******************************************************************************/
void BSP_SystemReset(void)
{
    NVIC_SystemReset();
}


