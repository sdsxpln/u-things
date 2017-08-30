#include "systick.h"
#include "log.h"

/* us */
volatile uint64_t systick = 0;

static volatile uint32_t TimingDelay;
 
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

void udelay(volatile uint32_t tus)
{ 
	//TimingDelay = nTime;	

	// ʹ�ܵδ�ʱ��  
	//SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;

	//while(TimingDelay != 0);
	uint64_t tick;
	tus = tus <= 1000000 ? tus : 1000000;

	tick = (systick + ((SysTick->LOAD - SysTick->VAL) / (SystemCoreClock / 1000000)));

	while(1) {
		if ((systick + ((SysTick->LOAD - SysTick->VAL) / (SystemCoreClock / 1000000)) - tick) >= tus) {
			break;
		}
	}
}

#if 0
void mdelay(volatile uint32_t tms)
{
	uint32_t i;
	for(i = 0; i < tms; i++) {
		udelay(1000);
	}
}
#endif

uint64_t get_systick()
{
	return ((systick + ((SysTick->LOAD - SysTick->VAL) / (SystemCoreClock / 1000000))));
}

void SysTick_Handler(void)
{
  //PRINT_EMG("%s-%d \n", __func__, __LINE__);
  //void TimingDelay_Decrement(void);
  //TimingDelay_Decrement();
  systick += SYSTICK_FREQ_US;
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
