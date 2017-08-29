#include "timer.h"
#include "dma.h"

int32_t dma_memcpy(void *dst, void *src, uint32_t size)
{
		uint32_t count = 0;
	
	  DMA_InitTypeDef DMA_InitStructure;
	
		// ����DMAʱ��
		RCC_AHBPeriphClockCmd(DMA_CLOCK, ENABLE);
		// Դ���ݵ�ַ
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)src;
		// Ŀ���ַ
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)dst;
		// �������赽�洢����������������ڲ���FLASH��	
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
		// �����С	
		DMA_InitStructure.DMA_BufferSize = size;
		// ���裨�ڲ���FLASH����ַ����	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
		// �ڴ��ַ����
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		// �������ݵ�λ	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		// �ڴ����ݵ�λ
		DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;	 
		// DMAģʽ��һ�λ���ѭ��ģʽ
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal ;
		//DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  
		// ���ȼ�����	
		DMA_InitStructure.DMA_Priority = DMA_Priority_High;
		// ʹ���ڴ浽�ڴ�Ĵ���
		DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
		// ����DMAͨ��		   
		DMA_Init(DMA_CHANNEL, &DMA_InitStructure);
    //���DMA������������ɱ�־λ
    DMA_ClearFlag(DMA_FLAG_TC);
		// ʹ��DMA
		DMA_Cmd(DMA_CHANNEL,ENABLE);
		
		/* block dma TODO: non-block dma */
		while(DMA_GetFlagStatus(DMA_FLAG_TC) == RESET) {
			mdelay(1);
			count++;
			
			if (count == 100) {
				return -1;
			}
		}
		
		return 0;
}

