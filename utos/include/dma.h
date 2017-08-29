#ifndef __DMA_H__
#define __DMA_H__

#include "stm32f10x.h"

// ��ʹ�ô洢�����洢��ģʽʱ��ͨ���������ѡ��û��Ӳ�ԵĹ涨
#define DMA_CHANNEL     DMA1_Channel6
#define DMA_CLOCK       RCC_AHBPeriph_DMA1

// ������ɱ�־
#define DMA_FLAG_TC     DMA1_FLAG_TC6

int32_t dma_memcpy(void *dst, void *src, uint32_t size);

#endif /* __DMA_H__ */
