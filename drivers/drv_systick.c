/**
  ******************************************************************************
  * @file    drv_systick.c
  * @author  mousie
  * @version V2.0.0
  * @date    2011.12.15
  * @brief   ϵͳ���ʱ��, ���ṩ΢��ͺ�����ʱ����.
  ******************************************************************************
  */

/* Include -------------------------------------------------------------------*/
#include "drv_systick.h"

#include "misc.h"

/** @addtogroup Drivers
  * @{
  */
/** @defgroup DRV_SysTick
  * @{
  */



/**
  ******************************************************************************
  * @addtogroup DRV_SysTick_Configure
  ******************************************************************************
  * @{
  */
#include "stm32f10x.h"
#define systick_enable_int(mask)        __set_PRIMASK(mask)
#define systick_disable_int()           __get_PRIMASK(); __disable_irq()
/**
  * @}
  */



/** @defgroup DRV_SysTick_Private_TypeDefine
  * @brief    ˽�����Ͷ���
  * @{
  */

/**
  * @}
  */

/** @defgroup DRV_SysTick_Private_MacroDefine
  * @brief    ˽�к궨��
  * @{
  */

/**
  * @}
  */

/** @defgroup DRV_SysTick_Variables
  * @brief    ����ȫ�ֱ���(˽��/����)
  * @{
  */
static volatile int32_t timer_delay_ms = 0;                                       ///< ������ʱ������
/**
  * @}
  */

/** @defgroup DRV_SysTick_Private_Function
  * @brief    ����˽�к���
  * @{
  */

/**
  * @}
  */



/** @defgroup DRV_SysTick_Function
  * @brief    ����ԭ�ļ�
  * @{
  */

/**
  ******************************************************************************
  * @brief  ϵͳ���ʱ�ӳ�ʼ��Ϊ1ms����. ���жϴ�������"irq_handler.c".
  * @param  none
  * @retval none
  ******************************************************************************
  */
void systick_init_1ms(void)
{
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    SysTick_Config(SystemCoreClock / 1000);
}

/**
  ******************************************************************************
  * @brief  ��ʱ����1msɨ��, �������Ҫ�ô˺���. ������ڶ�ʱ���ж���.
  * @param  none
  * @retval none
  ******************************************************************************
  */
void delay_scan_1ms(void)
{
    timer_delay_ms++;
}

/**
  ******************************************************************************
  * @brief  ��ȡ1ms��ʱʱ��ֵ.
  * @param  none
  * @retval none
  ******************************************************************************
  */
uint32_t delay_tick_get(void)
{
    return timer_delay_ms;
}

/**
  ******************************************************************************
  * @brief  ������ʱ����
  * @param  time, Ҫ��ʱ�ĺ���ʱ��. �����+10us֮��. �ú������������ж���ʹ��!
  * @retval none
  ******************************************************************************
  */
void delay_ms(uint16_t time)
{
    uint32_t timer_delay_ms_head;
    uint32_t timer_delay_ms_tail;
    uint32_t timer_delay_ms_cur;
    uint32_t timer_delay_tick_head;
    uint32_t mask;

    if (time)
    {
        mask = systick_disable_int();
        timer_delay_tick_head = SysTick->VAL;
        timer_delay_ms_head = timer_delay_ms;
        systick_enable_int(mask);
        timer_delay_ms_cur  = timer_delay_ms_head;
        timer_delay_ms_tail = timer_delay_ms_head + time;

        if (timer_delay_ms_tail >= timer_delay_ms_head)                         // ��һֱ��, Ŀ��ʱ������ʼʱ����ұ�
        {
            while((timer_delay_ms_cur >= timer_delay_ms_head) && (timer_delay_ms_cur <= timer_delay_ms_tail))
            {
                mask = systick_disable_int();
                timer_delay_ms_cur = timer_delay_ms;
                if ((timer_delay_ms_cur == timer_delay_ms_tail) && (SysTick->VAL <= timer_delay_tick_head))
                {
                    systick_enable_int(mask);
                    break;
                }
                systick_enable_int(mask);
            }
        }
        else                                                                    // ��һֱ��, Ŀ��ʱ������ʼʱ������
        {
            while((timer_delay_ms_cur >= timer_delay_ms_head) || (timer_delay_ms <= timer_delay_ms_tail))
            {
                mask = systick_disable_int();
                timer_delay_ms_cur = timer_delay_ms;
                if ((timer_delay_ms_cur == timer_delay_ms_tail) && (SysTick->VAL <= timer_delay_tick_head))
                {
                    systick_enable_int(mask);
                    break;
                }
                systick_enable_int(mask);
            }
        }
    }
}

/**
  ******************************************************************************
  * @brief  ΢����ʱ����
  * @param  time, Ҫ��ʱ��΢��ʱ��. <1000usʱ, �����+5us֮��. �ú������������ж���ʹ��!
  * @retval none
  ******************************************************************************
  */
void delay_us(uint16_t time)
{
  uint32_t timer_delay_tick_head;
  uint32_t timer_delay_tick_tail;
  uint32_t timer_delay_tick_cur;
  uint32_t timer_delay_ms_tail;
  uint32_t mask;

  timer_delay_tick_head = SysTick->VAL;
  delay_ms(time / 1000);
  mask = systick_disable_int();
  timer_delay_ms_tail   = timer_delay_ms + time/1000 + 1;
  systick_enable_int(mask);
  timer_delay_tick_tail = (time % 1000) * (SystemCoreClock / 1000000);
  timer_delay_tick_cur  = SysTick->VAL;

  if (timer_delay_tick_tail <= timer_delay_tick_head)
  {
    timer_delay_tick_tail = timer_delay_tick_head - timer_delay_tick_tail;
    while((timer_delay_tick_cur < timer_delay_tick_head) && (timer_delay_tick_cur > timer_delay_tick_tail))
    {
      timer_delay_tick_cur = SysTick->VAL;
      mask = systick_disable_int();
      if ((timer_delay_ms > timer_delay_ms_tail) || (timer_delay_ms < (timer_delay_ms_tail - 1)))
      {
        systick_enable_int(mask);
        break;
      }
      systick_enable_int(mask);
    }
  }
  else
  {
    timer_delay_tick_tail = SysTick->LOAD + timer_delay_tick_head - timer_delay_tick_tail - 1;
    while((timer_delay_tick_cur < timer_delay_tick_head) || (timer_delay_tick_cur > timer_delay_tick_tail))
    {
      timer_delay_tick_cur = SysTick->VAL;
      mask = systick_disable_int();
      if ((timer_delay_ms > timer_delay_ms_tail) || (timer_delay_ms < (timer_delay_ms_tail - 1)))
      {
        systick_enable_int(mask);
        break;
      }
      systick_enable_int(mask);
    }
  }
}

/**
  * @}
  */



/**
  * @}
  */
/**
  * @}
  */

/* END OF FILE ---------------------------------------------------------------*/
