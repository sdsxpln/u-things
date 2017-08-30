
#include "timer.h"
#include "gpio.h"

/* 1ms counter */
volatile uint64_t time = 0;

#if 0
void udelay(uint32_t tick)
{
    volatile uint32_t t, i, x;

	for(t = 0; t < tick; t++) {
		for(i = 0; i < 1; i++) {

			for(x = 0; x < 10; x++) {
				__asm volatile ("nop");
			}
		}
	}
}
#endif

void mdelay(uint32_t tick)
{
    uint64_t old_time = time;

    tick = tick <= 1000000 ? tick : 1000000;

    while(1) {
        if ((time - old_time) >= tick) {
            break;
        }
    }

}

// �ж����ȼ�����
static void ADVANCE_TIM_NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // �����ж���Ϊ0
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);		
		// �����ж���Դ
    NVIC_InitStructure.NVIC_IRQChannel = ADVANCE_TIM_IRQ ;	
		// ���������ȼ�Ϊ 0
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	  // ������ռ���ȼ�Ϊ3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

///*
// * ע�⣺TIM_TimeBaseInitTypeDef�ṹ��������5����Ա��TIM6��TIM7�ļĴ�������ֻ��
// * TIM_Prescaler��TIM_Period������ʹ��TIM6��TIM7��ʱ��ֻ���ʼ����������Ա���ɣ�
// * ����������Ա��ͨ�ö�ʱ���͸߼���ʱ������.
// *-----------------------------------------------------------------------------
// *typedef struct
// *{ TIM_Prescaler            ����
// *	TIM_CounterMode			     TIMx,x[6,7]û�У���������
// *  TIM_Period               ����
// *  TIM_ClockDivision        TIMx,x[6,7]û�У���������
// *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17]����
// *}TIM_TimeBaseInitTypeDef; 
// *-----------------------------------------------------------------------------
// */
static void ADVANCE_TIM_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;		
		// ������ʱ��ʱ��,���ڲ�ʱ��CK_INT=72M
    ADVANCE_TIM_APBxClock_FUN(ADVANCE_TIM_CLK, ENABLE);	
		// �Զ���װ�ؼĴ�����ֵ���ۼ�TIM_Period+1��Ƶ�ʺ����һ�����»����ж�
    TIM_TimeBaseStructure.TIM_Period=ADVANCE_TIM_Period;
	  // ʱ��Ԥ��Ƶ��
    TIM_TimeBaseStructure.TIM_Prescaler= ADVANCE_TIM_Prescaler;	
		// ʱ�ӷ�Ƶ���� ��û�õ����ù�
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
		// ����������ģʽ������Ϊ���ϼ���
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		
		// �ظ���������ֵ��û�õ����ù�
		TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
	  // ��ʼ����ʱ��
    TIM_TimeBaseInit(ADVANCE_TIM, &TIM_TimeBaseStructure);
	
		// ����������жϱ�־λ
    TIM_ClearFlag(ADVANCE_TIM, TIM_FLAG_Update);
	  
		// �����������ж�
    TIM_ITConfig(ADVANCE_TIM,TIM_IT_Update,ENABLE);
		
		// ʹ�ܼ�����
    TIM_Cmd(ADVANCE_TIM, ENABLE);
}

void  ADVANCE_TIM_IRQHandler (void)
{
    if ( TIM_GetITStatus( ADVANCE_TIM, TIM_IT_Update) != RESET ) 
    {   
        time++;
		
		/* led blink */
		if (time % 1000) {
			gpio_write(GROUPB, 1, (time / 1000) % 2);
		}
		
        TIM_ClearITPendingBit(ADVANCE_TIM , TIM_FLAG_Update);        
    }           
}

uint64_t get_time()
{
	return time;
}

void timer_init()
{
	ADVANCE_TIM_NVIC_Config();
	ADVANCE_TIM_Mode_Config();		
}

