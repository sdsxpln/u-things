/**
  ******************************************************************************
  * @file    bsp_SysTick.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   SysTick ϵͳ�δ�ʱ��10us�жϺ�����,�ж�ʱ����������ã�
  *          ���õ��� 1us 10us 1ms �жϡ�     
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:���� F103-ָ���� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

#include <libc.h>
#include "systick.h"
#include "log.h"

/* us */
volatile __u64 systick = 0;

static volatile u32 TimingDelay;
 
/**
  * @brief  ����ϵͳ�δ�ʱ�� SysTick
  * @param  ��
  * @retval ��
  */
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
//	if (SysTick_Config(SystemFrequency / 100000))	// ST3.0.0��汾

	if (SysTick_Config(SYSTICK_FREQ_US * (SystemCoreClock / 1000000)))	// 100ms int ST3.5.0��汾
	{ 
		PRINT_EMG("%s-%d %x fail\n", __func__, __LINE__, SystemCoreClock);
	}
		// �رյδ�ʱ��  
	//SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}

void udelay(volatile u32 tus)
{ 
	//TimingDelay = nTime;	

	// ʹ�ܵδ�ʱ��  
	//SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;

	//while(TimingDelay != 0);
	u32 tick;
	tus = tus <= 1000000 ? tus : 1000000;

	tick = (systick + ((SysTick->LOAD - SysTick->VAL) / (SystemCoreClock / 1000000)));

	while(1) {
		if ((systick + ((SysTick->LOAD - SysTick->VAL) / (SystemCoreClock / 1000000)) - tick) >= tus) {
			break;
		}
	}
}

/**
  * @brief  ��ȡ���ĳ���
  * @param  ��
  * @retval ��
  * @attention  �� SysTick �жϺ��� SysTick_Handler()����
  */
void TimingDelay_Decrement(void)
{
	if (TimingDelay != 0x00)
	{ 
		TimingDelay--;
	}
}
/*********************************************END OF FILE**********************/
