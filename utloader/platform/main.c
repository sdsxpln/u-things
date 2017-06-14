#include "stm32f10x.h"
#include "usart.h"
#include "log.h"
#include "libc.h"

s32 _assert(const char *file_name, const char *func_name, u32 line_num, char *desc)
{
    PRINT_EMG("[%s][%s][%d]: %s\n", file_name, func_name, line_num, desc);
    while(1);
}


/*���ļ�������ӼĴ�����ַ���ṹ�嶨��*/

/*Ƭ���������ַ  */
#define PERIPH_BASE           ((unsigned int)0x40000000)

/*APB2 ���߻���ַ */
#define APB2PERIPH_BASE       (PERIPH_BASE + 0x10000)
/* AHB���߻���ַ */
#define AHBPERIPH_BASE        (PERIPH_BASE + 0x20000)

/*GPIOB�������ַ*/
#define GPIOB_BASE            (APB2PERIPH_BASE + 0x0C00)

/* GPIOB�Ĵ�����ַ,ǿ��ת����ָ�� */
#define GPIOB_CRL           *(unsigned int*)(GPIOB_BASE+0x00)
#define GPIOB_CRH           *(unsigned int*)(GPIOB_BASE+0x04)
#define GPIOB_IDR           *(unsigned int*)(GPIOB_BASE+0x08)
#define GPIOB_ODR           *(unsigned int*)(GPIOB_BASE+0x0C)
#define GPIOB_BSRR    *(unsigned int*)(GPIOB_BASE+0x10)
#define GPIOB_BRR           *(unsigned int*)(GPIOB_BASE+0x14)
#define GPIOB_LCKR      *(unsigned int*)(GPIOB_BASE+0x18)

/*RCC�������ַ*/
#define RCC_BASE      (AHBPERIPH_BASE + 0x1000)
/*RCC��AHB1ʱ��ʹ�ܼĴ�����ַ,ǿ��ת����ָ��*/
#define RCC_APB2ENR      *(unsigned int*)(RCC_BASE+0x18)

int main(void)
{	
#if 0
    USART_Config();

    Usart_SendString( DEBUG_USARTx,"test1111111111\n");
    printf("welcome\n\n\n");

    while(1)
    {   
        printf("test123\n");
        printf("%d %d \n", g_1, g_2);
    }   
#endif

    // ����GPIOB �˿�ʱ��
    RCC_APB2ENR |= (1<<3);

    //��տ���PB0�Ķ˿�λ
    GPIOB_CRL &= ~( 0x0F<< (4*0));  
    // ����PB0Ϊͨ������������ٶ�Ϊ10M
    GPIOB_CRL |= (1<<4*0);

    // PB0 ��� �͵�ƽ
    GPIOB_ODR &= ~(1<<0);
    
    USART_Config();

    Usart_SendString( DEBUG_USARTx,"test1111111111\n");

    while(1) {
        
    }

    while(1);
}

