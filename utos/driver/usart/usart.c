/**
  ******************************************************************************
  * @file    bsp_usart.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   �ض���c��printf������usart�˿�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:����STM32 F103-ָ���� ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "usart.h"
#include "watchdog.h"
#include "shell.h"

int uart1_printf(const char *format, ...);

int uart_work_mode = SHELL_MODE;

 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Ƕ�������жϿ�������ѡ�� */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* ����USARTΪ�ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
  /* �������ȼ�*/
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* �����ȼ� */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* ʹ���ж� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* ��ʼ������NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

 /**
  * @brief  USART GPIO ����,������������
  * @param  ��
  * @retval ��
  */
void USART_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// �򿪴���GPIO��ʱ��
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	
	// �򿪴��������ʱ��
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

	// ��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  // ��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// ���ô��ڵĹ�������
	// ���ò�����
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	// ���� �������ֳ�
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// ����ֹͣλ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// ����У��λ
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// ����Ӳ��������
	USART_InitStructure.USART_HardwareFlowControl = 
	USART_HardwareFlowControl_None;
	// ���ù���ģʽ���շ�һ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// ��ɴ��ڵĳ�ʼ������
	USART_Init(DEBUG_USARTx, &USART_InitStructure);
	
	// �����ж����ȼ�����
	NVIC_Configuration();
	
	// ʹ�ܴ��ڽ����ж�
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);	
	
	// ʹ�ܴ���
	USART_Cmd(DEBUG_USARTx, ENABLE);	    
}

/*****************  ����һ���ֽ� **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/****************** ����8λ������ ************************/
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    /* ����һ���ֽ����ݵ�USART */
	    Usart_SendByte(pUSARTx,array[i]);	
  
  }
	/* �ȴ�������� */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}

/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      if (*(str + k) == '\n') {
          /* unix style, we send the extra '\r' */
        Usart_SendByte( pUSARTx, '\r');
      }

      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* �ȴ�������� */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  ����һ��16λ�� **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* ȡ���߰�λ */
	temp_h = (ch&0XFF00)>>8;
	/* ȡ���Ͱ�λ */
	temp_l = ch&0XFF;
	
	/* ���͸߰�λ */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* ���͵Ͱ�λ */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

#include <libc.h>
#include "log.h"
#define UART_IO_SIZE 256
/* SHELL_MODE */
volatile __u32 uart_recv_buf_index = 0;
/* YMODEM_MODE */
volatile __u32 first = 0; 
volatile __u32 last  = 0;

volatile char uart_recv_buf[UART_IO_SIZE] = {0};

void uart_putc(__u8 byte) 
{
	Usart_SendByte(DEBUG_USARTx, byte);
}

void uart_puts(char *str)
{
	Usart_SendString(DEBUG_USARTx, str);
}

int uart_printf(const char *format, ...)
{
    u32 len;
    va_list args;
    static char format_buf[UART_IO_SIZE] = {0};

    va_start(args, format);
    len = vsnprintf(format_buf, sizeof(format_buf), format, args);
    va_end(args);

    uart_puts(format_buf);

    return len;
}


void DEBUG_USART_IRQHandler(void)
{
    static __u8 i, magic_cmd[6] = {0};

	uint16_t ch;
	ch = (__u8)USART_ReceiveData(DEBUG_USARTx);
	uart1_printf("%s-%d %x \n", __func__, __LINE__, ch);

    for(i = 0; i < 4; i++) {
        magic_cmd[i] = magic_cmd[i + 1];
    }
    magic_cmd[4] = ch;

    if (magic_cmd[0] == 'r' && 
        magic_cmd[1] == 'e' &&
        magic_cmd[2] == 's' &&
        magic_cmd[3] == 'e' &&
        magic_cmd[4] == 't') {
        uart_printf("magic cmd [%s]!\n", magic_cmd);
        watchdog_reset();
    }

	/* uart_printf("enter %s-%d %x \n", __func__, __LINE__, ch); */

    switch (uart_work_mode) {
        case (SHELL_MODE):
            if (ch == '\n') {   /* sscom will send '\r\n' we ignore the '\n' */
                return;
            }
            if (uart_recv_buf_index == (UART_IO_SIZE - 1) && ch != '\r') {
                uart_puts("cmd too long!\n");
                uart_recv_buf_index = 0;
                return;

            }

            if (ch == '\r') {
                uart_recv_buf[uart_recv_buf_index] = '\0';  /* terminate the string. */
				/* FIXME: uart_recv_buf is share by irq handler (in int context) and shell handler (in thread context) */
                shell_cmd = uart_recv_buf;
                /* shell(uart_recv_buf); */

                uart_recv_buf_index = 0;
                /* uart_puts("\nutloader>"); */
                return;
            } else {
                uart_recv_buf[uart_recv_buf_index] = ch;
                uart_recv_buf_index++;
            }

            /* echo */
            uart_putc(ch);
            break;
        case (YMODEM_MODE):
            if ((last + 1) % UART_IO_SIZE == first) {
                uart_puts("buf full!\n");
				return;
            }
            uart1_printf("uart produce %x\n", ch);
            uart_recv_buf[last++] = ch;

            if (last == UART_IO_SIZE) {
                last = 0;
            }
            break;
        default:    /* FIXME: panic() */
            break;
    }
}

/* only for YMODEM_MODE */

/* 0: no data; 1: has data; */ 
int uart_fifo_status()
{
    if (first == last) {
        return 0;
    } else {
        return 1;
    }
}

char uart_recv()
{
    char ch;
    if (uart_fifo_status() == 1) {
        ch = uart_recv_buf[first++];
        uart1_printf("uart consume %x\n", ch);
        if (first == UART_IO_SIZE) {
            first = 0;
        }
		return ch;
    } else {
        return 0;
    }
}

#if 1
/* for uart1 */

int uart1_status = 0;

void uart1_init()
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;
	
	  // �򿪴���GPIO��ʱ��
	  DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART1_GPIO_CLK, ENABLE);
	  
	  // �򿪴��������ʱ��
	  DEBUG_USART_APBxClkCmd(DEBUG_USART1_CLK, ENABLE);
	
	  // ��USART Tx��GPIO����Ϊ���츴��ģʽ
	  GPIO_InitStructure.GPIO_Pin = DEBUG_USART1_TX_GPIO_PIN;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(DEBUG_USART1_TX_GPIO_PORT, &GPIO_InitStructure);
	
	// ��USART Rx��GPIO����Ϊ��������ģʽ
	  GPIO_InitStructure.GPIO_Pin = DEBUG_USART1_RX_GPIO_PIN;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	  GPIO_Init(DEBUG_USART1_RX_GPIO_PORT, &GPIO_InitStructure);
	  
	  // ���ô��ڵĹ�������
	  // ���ò�����
	  USART_InitStructure.USART_BaudRate = DEBUG_USART1_BAUDRATE;
	  // ���� �������ֳ�
	  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	  // ����ֹͣλ
	  USART_InitStructure.USART_StopBits = USART_StopBits_1;
	  // ����У��λ
	  USART_InitStructure.USART_Parity = USART_Parity_No ;
	  // ����Ӳ��������
	  USART_InitStructure.USART_HardwareFlowControl = 
	  USART_HardwareFlowControl_None;
	  // ���ù���ģʽ���շ�һ��
	  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	  // ��ɴ��ڵĳ�ʼ������
	  USART_Init(DEBUG_USART1, &USART_InitStructure);
	  
	  // �����ж����ȼ�����
	  //NVIC_Configuration();
	  NVIC_InitTypeDef NVIC_InitStructure;
	  
	  /* Ƕ�������жϿ�������ѡ�� */
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	  
	  /* ����USARTΪ�ж�Դ */
	  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART1_IRQ;
	  /* �������ȼ�*/
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	  /* �����ȼ� */
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	  /* ʹ���ж� */
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  /* ��ʼ������NVIC */
	  NVIC_Init(&NVIC_InitStructure);

	  
	  // ʹ�ܴ��ڽ����ж�
	  USART_ITConfig(DEBUG_USART1, USART_IT_RXNE, ENABLE);	  
	  
	  // ʹ�ܴ���
	  USART_Cmd(DEBUG_USART1, ENABLE);	  
	  uart1_status = 1;

}

int uart1_printf(const char *format, ...)
{
    __u32 len;
    va_list args;
    static char format_buf[UART_IO_SIZE] = {0};
	
	if (uart1_status == 0) {
		return 0;
	}
	
    va_start(args, format);
    len = vsnprintf(format_buf, sizeof(format_buf), format, args);
    va_end(args);

    Usart_SendString(DEBUG_USART1, format_buf);
    return len;

}

void DEBUG_USART1_IRQHandler(void)
{
	uart1_printf("%s-%d %x\n", __func__, __LINE__, (__u8)USART_ReceiveData(DEBUG_USART1));
}

#endif


#if 1
/* for uart2 */



void uart2_init()
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;
	
	  // �򿪴���GPIO��ʱ��
	  DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART2_GPIO_CLK, ENABLE);
	  
	  // �򿪴��������ʱ��
	  DEBUG_USART_APBxClkCmd(DEBUG_USART2_CLK, ENABLE);
	
	  // ��USART Tx��GPIO����Ϊ���츴��ģʽ
	  GPIO_InitStructure.GPIO_Pin = DEBUG_USART2_TX_GPIO_PIN;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(DEBUG_USART2_TX_GPIO_PORT, &GPIO_InitStructure);
	
	// ��USART Rx��GPIO����Ϊ��������ģʽ
	  GPIO_InitStructure.GPIO_Pin = DEBUG_USART2_RX_GPIO_PIN;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	  GPIO_Init(DEBUG_USART2_RX_GPIO_PORT, &GPIO_InitStructure);
	  
	  // ���ô��ڵĹ�������
	  // ���ò�����
	  USART_InitStructure.USART_BaudRate = DEBUG_USART2_BAUDRATE;
	  // ���� �������ֳ�
	  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	  // ����ֹͣλ
	  USART_InitStructure.USART_StopBits = USART_StopBits_1;
	  // ����У��λ
	  USART_InitStructure.USART_Parity = USART_Parity_No ;
	  // ����Ӳ��������
	  USART_InitStructure.USART_HardwareFlowControl = 
	  USART_HardwareFlowControl_None;
	  // ���ù���ģʽ���շ�һ��
	  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	  // ��ɴ��ڵĳ�ʼ������
	  USART_Init(DEBUG_USART2, &USART_InitStructure);
	  
	  // �����ж����ȼ�����
	  //NVIC_Configuration();
	  NVIC_InitTypeDef NVIC_InitStructure;
	  
	  /* Ƕ�������жϿ�������ѡ�� */
	  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	  
	  /* ����USARTΪ�ж�Դ */
	  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART2_IRQ;
	  /* �������ȼ�*/
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	  /* �����ȼ� */
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	  /* ʹ���ж� */
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  /* ��ʼ������NVIC */
	  NVIC_Init(&NVIC_InitStructure);

	  
	  // ʹ�ܴ��ڽ����ж�
	  USART_ITConfig(DEBUG_USART2, USART_IT_RXNE, ENABLE);	  
	  
	  // ʹ�ܴ���
	  USART_Cmd(DEBUG_USART2, ENABLE);	  

}

int uart2_printf(const char *format, ...)
{
    __u32 len;
    va_list args;
    static char format_buf[UART_IO_SIZE] = {0};
	
    va_start(args, format);
    len = vsnprintf(format_buf, sizeof(format_buf), format, args);
    va_end(args);

    Usart_SendString(DEBUG_USART2, format_buf);
    return len;

}

void DEBUG_USART2_IRQHandler(void)
{
	uart1_printf("%s-%d %x\n", __func__, __LINE__, (__u8)USART_ReceiveData(DEBUG_USART2));
}

#endif
void uart_init()
{
	uart2_init();
}

#if 0
///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ����� */
		USART_SendData(DEBUG_USARTx, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		/* �ȴ������������� */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USARTx);
}
#endif
