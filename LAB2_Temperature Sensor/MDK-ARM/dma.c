#include "dma.h"

DMA_InitTypeDef dma_Init;
DMA_HandleTypeDef hdma;

void DMA_Init(void)
{
	//InitTypeDef setting
	dma_Init.Channel = DMA_CHANNEL_0;
	dma_Init.Direction = DMA_PERIPH_TO_MEMORY;
	dma_Init.PeriphInc = DMA_PINC_ENABLE;
	dma_Init.MemInc = DMA_MINC_ENABLE;
	dma_Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
	dma_Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
	dma_Init.Mode = DMA_CIRCULAR;
	dma_Init.Priority = DMA_PRIORITY_VERY_HIGH;
	dma_Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	//dma_Init.FIFOThreshold = DMA
	
	//HandleTypeDef setting
	hdma.Instance = DMA2_Stream0;
	hdma.Init = dma_Init;
	
	HAL_DMA_Init(&hdma);
	
	//__HAL_LINKDMA();
	
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn,0,0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	
}