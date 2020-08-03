/*
********************************************************************************
*
*                                 BSP_Tim.c
*
* File          : BSP_Tim.c
* Version       : V1.0
* Author        : whq
* Mode          : Thumb2
* Toolchain     : 
* Description   : 定时器 驱动程序
*                
* History       :
* Date          : 2013.07.25
*******************************************************************************/

#include "misc.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"


#include "USB/inc/BSP_Tim.h"


static void(* TIM_CALLBACK[TIMn])(void) = {(void *)0};

static TIM_TypeDef* const TIM_NUM[TIMn] = {
#if TIM_1_EN
    BSP_TIM1,
#endif
#if TIM_2_EN
    BSP_TIM2,
#endif
#if TIM_3_EN
    BSP_TIM3,
#endif
#if TIM_4_EN
    BSP_TIM4,
#endif
#if TIM_5_EN
    BSP_TIM5,
#endif
#if TIM_6_EN
    BSP_TIM6,
#endif
#if TIM_7_EN
    BSP_TIM7,
#endif
#if TIM_8_EN
    BSP_TIM8,
#endif
}; 
                                            
static const uint32_t TIM_CLK[TIMn] = {
#if TIM_1_EN
    BSP_TIM1_CLK,
#endif
#if TIM_2_EN
    BSP_TIM2_CLK,
#endif
#if TIM_3_EN
    BSP_TIM3_CLK,
#endif
#if TIM_4_EN
    BSP_TIM4_CLK,
#endif
#if TIM_5_EN
    BSP_TIM5_CLK,
#endif
#if TIM_6_EN
    BSP_TIM6_CLK,
#endif
#if TIM_7_EN
    BSP_TIM7_CLK,
#endif
#if TIM_8_EN
    BSP_TIM8_CLK,
#endif
};
                                    
static const uint8_t TIM_IRQn[TIMn] = {
#if TIM_1_EN
    BSP_TIM1_IRQn,
#endif
#if TIM_2_EN
    BSP_TIM2_IRQn,
#endif
#if TIM_3_EN
    BSP_TIM3_IRQn,
#endif
#if TIM_4_EN
    BSP_TIM4_IRQn,
#endif
#if TIM_5_EN
    BSP_TIM5_IRQn,
#endif
#if TIM_6_EN
    BSP_TIM6_IRQn,
#endif
#if TIM_7_EN
    BSP_TIM7_IRQn,
#endif
#if TIM_8_EN
    BSP_TIM8_IRQn,
#endif
};


/*******************************************************************************
* Function Name : void BSP_TimOpen(uint8_t TIM_x, uint32_t autoreload, FunVoidVoid *CallBack)
* Description   : 定时器打开
* Input         :   TIM_x: TIM_1~ TIM_8
                    prescaler:  时钟预分频值
                    period:     时钟重载值
* Output        : 
* Other         :   分频值，重载值，内部以做减1处理。(外部无需减1)
* Date          : 2013.07.25
*******************************************************************************/
void BSP_TimOpen(uint8_t TIM_x, uint32_t prescaler, uint32_t autoreload, void(*CallBack)(void))
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable TIM clock */
#ifdef STM32F10X_HD
    if ((TIM_NUM[TIM_x] == TIM1) || (TIM_NUM[TIM_x] == TIM8))
        RCC_APB2PeriphClockCmd(TIM_CLK[TIM_x], ENABLE); 
#else
    if (TIM_NUM[TIM_x] == TIM1)
        RCC_APB2PeriphClockCmd(TIM_CLK[TIM_x], ENABLE); 
#endif
    else
        RCC_APB1PeriphClockCmd(TIM_CLK[TIM_x], ENABLE);

    /* Time Base configuration */
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = autoreload? (autoreload - 1) : autoreload;
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler? (prescaler - 1) : prescaler;    
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM_NUM[TIM_x], &TIM_TimeBaseStructure);

    /* Enable the TIM global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = TIM_IRQn[TIM_x];
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0xF;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //装载回调函数
    TIM_CALLBACK[TIM_x] = CallBack;
    
    //使能 预加载寄存器
    TIM_ARRPreloadConfig(TIM_NUM[TIM_x], ENABLE);
    
    /* TIM enable counter */
    TIM_Cmd(TIM_NUM[TIM_x], ENABLE);

    /* Enable  Interrupt Request */
    TIM_ITConfig(TIM_NUM[TIM_x], TIM_IT_Update, ENABLE);
}


/*******************************************************************************
* Function Name : void BSP_TimClose(TIM_x)
* Description   : TIM_x: TIM_1~ TIM_8
* Input         : 
* Output        : 
* Other         : 
* Date          : 2013.07.25
*******************************************************************************/
void BSP_TimClose(uint8_t TIM_x)
{
    /* Enable TIM clock */
#ifdef STM32F10X_HD
    if ((TIM_NUM[TIM_x] == TIM1) || (TIM_NUM[TIM_x] == TIM8))
        RCC_APB2PeriphClockCmd(TIM_CLK[TIM_x], DISABLE); 
#else
    if (TIM_NUM[TIM_x] == TIM1)
        RCC_APB2PeriphClockCmd(TIM_CLK[TIM_x], DISABLE); 
#endif
        RCC_APB1PeriphClockCmd(TIM_CLK[TIM_x], DISABLE);

    TIM_Cmd(TIM_NUM[TIM_x], DISABLE);

    TIM_ITConfig(TIM_NUM[TIM_x], TIM_IT_Update, DISABLE);  

    //清空回调函数
    TIM_CALLBACK[TIM_x] = (void *)0;
}

/*******************************************************************************
* Function Name : void BSP_TimAutoreloadSet(uint8_t TIM_x, uint32_t autoreload)
* Description   : 设置重载值
* Input         :   TIM_x:      TIM_2-TIM_14
                    autoreload  重载值
* Output        : 
* Other         : 
* Date          : 2013.08.16
*******************************************************************************/
void BSP_TimAutoreloadSet(uint8_t TIM_x, uint32_t autoreload)
{
    TIM_SetAutoreload(TIM_NUM[TIM_x], autoreload);

}

/*******************************************************************************
* Function Name : uint32_t BSP_TimCounterGet(uint8_t TIM_x)
* Description   : 获取计数值
* Input         : TIM_x:        TIM_2-TIM_14
* Output        : 
* Other         : 
* Date          : 2013.08.16
*******************************************************************************/
uint32_t BSP_TimCounterGet(uint8_t TIM_x)
{
    return TIM_GetCounter(TIM_NUM[TIM_x]);
}

/*******************************************************************************
* Function Name : void BSP_TimCounterSet(uint8_t TIM_x, uint32_t Counter)
* Description   : 设定计数值
* Input         :   TIM_x:      TIM_2-TIM_14
                    Counter     设定计数值
* Output        : 
* Other         : 
* Date          : 2013.08.16
*******************************************************************************/
void BSP_TimCounterSet(uint8_t TIM_x, uint32_t Counter)
{
    TIM_SetCounter(TIM_NUM[TIM_x], Counter);
}

/*******************************************************************************
* Function Name : uint32_t BSP_TimClockGet(uint8_t TIM_x)
* Description   : 获取指定定时器时钟频率
* Input         : TIM_x:      TIM_2-TIM_14
* Output        : 
* Other         : 
* Date          : 2013.08.16
*******************************************************************************/
uint32_t BSP_TimClockGet(void)
{
    return (SystemCoreClock);
}

/*******************************************************************************
* Function Name : void BSP_TimITStateSet(uint8_t TIM_x, uint8_t state)
* Description   : 设置定时器 中断状态
* Input         : state :   0)关中断    1)开中断
* Output        : 
* Other         : 
* Date          : 2013.08.16
*******************************************************************************/
void BSP_TimITStateSet(uint8_t TIM_x, uint8_t state)
{
    TIM_ITConfig(TIM_NUM[TIM_x], TIM_IT_Update, ENABLE);
}

/*******************************************************************************
* Function Name : void BSP_TimIRQCallbackSet(uint8_t TIM_x, void(*CallBack)(void))
* Description   : 设定中断回调函数
* Input         : 
* Output        : 
* Other         : 
* Date          : 2013.08.16
*******************************************************************************/
void BSP_TimIRQCallbackSet(uint8_t TIM_x, void(*CallBack)(void))
{
    TIM_CALLBACK[TIM_x] = CallBack;
}

/*******************************************************************************
* Function Name : static void _UpIRQHandler(TIM_x)
* Description   : 中断处理程序
* Input         : TIM_x: TIM_1~ TIM_8
* Output        : 
* Other         : 
* Date          : 2013.07.25
*******************************************************************************/
static void _UpIRQHandler(uint8_t TIM_x)
{
    if (TIM_GetITStatus(TIM_NUM[TIM_x], TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM_NUM[TIM_x], TIM_IT_Update);
        if (TIM_CALLBACK[TIM_x]) TIM_CALLBACK[TIM_x]();
    }
}

#if TIM_1_EN
/*******************************************************************************
* Function Name  : TIM1_UP_IRQHandler
* Description    : This function handles TIM1 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_UP_IRQHandler(void)
{
    TIM_IRQEnter();
    _UpIRQHandler(TIM_1);
    TIM_IRQExit();
}
#endif 

#if TIM_2_EN
/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{
    TIM_IRQEnter();
    _UpIRQHandler(TIM_2);
    TIM_IRQExit();
}
#endif

#if TIM_3_EN
/*******************************************************************************
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_IRQHandler(void)
{    
    TIM_IRQEnter();
    _UpIRQHandler(TIM_3);
    TIM_IRQExit();
}
#endif

#if TIM_4_EN
/*******************************************************************************
* Function Name  : TIM4_IRQHandler
* Description    : This function handles TIM4 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_IRQHandler(void)
{
    TIM_IRQEnter();
    _UpIRQHandler(TIM_4);
    TIM_IRQExit();
}
#endif

#if TIM_5_EN
/*******************************************************************************
* Function Name  : TIM5_IRQHandler
* Description    : This function handles TIM5 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM5_IRQHandler(void)
{
    TIM_IRQEnter();
    _UpIRQHandler(TIM_5);
    TIM_IRQExit();
}
#endif

#if TIM_6_EN
/*******************************************************************************
* Function Name  : TIM6_IRQHandler
* Description    : This function handles TIM6 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM6_IRQHandler(void)
{
    TIM_IRQEnter();
    _UpIRQHandler(TIM_6);
    TIM_IRQExit();
}
#endif

#if TIM_7_EN
/*******************************************************************************
* Function Name  : TIM7_IRQHandler
* Description    : This function handles TIM7 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM7_IRQHandler(void)
{
    TIM_IRQEnter();
    _UpIRQHandler(TIM_7);
    TIM_IRQExit();
}
#endif

#if TIM_8_EN
/*******************************************************************************
* Function Name  : TIM8_UP_IRQHandler
* Description    : This function handles TIM8 overflow and update interrupt 
*                  request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_UP_IRQHandler(void)
{
    TIM_IRQEnter();
    _UpIRQHandler(TIM_8);
    TIM_IRQExit();
}
#endif


