/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUFFERSIZE 512
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint16_t ADCDualConvertedValues[BUFFERSIZE];
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);
/* Private functions ---------------------------------------------------------*/
void init_adc(void);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  // Setup status leds
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD, ENABLE );
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( GPIOD, &GPIO_InitStructure );

  init_adc();
  
  GPIO_SetBits( GPIOD, GPIO_Pin_15 );
  ADC_SoftwareStartConv( ADC1 );

  while(1);
}

//IRQ
void DMA2_Stream0_IRQHandler(void) {
  // Half transfer interrupt
  if( DMA_GetITStatus( DMA2_Stream0, DMA_IT_HTIF0 ) ) {
    DMA_ClearITPendingBit( DMA2_Stream0, DMA_IT_HTIF0 );
    GPIO_ResetBits( GPIOD, GPIO_Pin_15 );
  }

  if( DMA_GetITStatus( DMA2_Stream0, DMA_IT_TCIF0 ) ) {
    DMA_ClearITPendingBit( DMA2_Stream0, DMA_IT_TCIF0 );
    GPIO_SetBits( GPIOD, GPIO_Pin_15 );
  }
}

// init ADC
void init_adc() {
  // enable clocks
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC, ENABLE );
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM2, ENABLE );
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, ENABLE );

  // configure GPIO
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init( GPIOC, &GPIO_InitStructure );

  // setup interrupts
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( &NVIC_InitStructure );

  // setup tim2
  // freq on 70Khz
  TIM_TimeBaseInitTypeDef TIM_InitStructure;
  TIM_TimeBaseStructInit( &TIM_InitStructure );
  TIM_InitStructure.TIM_Period = (84000000 / 70000) - 1; 
  TIM_InitStructure.TIM_Prescaler = 0;
  TIM_InitStructure.TIM_ClockDivision = 0;
  TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit( TIM2, &TIM_InitStructure );
  // TIM2 TRGO Selection
  TIM_SelectOutputTrigger( TIM2, TIM_TRGOSource_Update );
  TIM_Cmd( TIM2, ENABLE );

  // dma configuration
  DMA_InitTypeDef DMA_InitStructure;
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &ADCDualConvertedValues;
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) 0x40012308;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = BUFFERSIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init( DMA2_Stream0, &DMA_InitStructure );
  // Enable DMA Stream Half/ Transfer Complete interrupt
  DMA_ITConfig( DMA2_Stream0, DMA_IT_TC | DMA_IT_HT, ENABLE );
  DMA_Cmd( DMA2_Stream0, ENABLE );

  // configure ADC
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  ADC_InitTypeDef ADC_InitStructure;

  ADC_CommonInitStructure.ADC_Mode = ADC_DualMode_RegSimult;
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_1; // 2 half words one by one
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit( &ADC_CommonInitStructure );
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T2_TRGO;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init( ADC1, &ADC_InitStructure );
  ADC_Init( ADC2, &ADC_InitStructure );
  // ADC1 Regular Channel 10
  ADC_RegularChannelConfig( ADC1, ADC_Channel_10, 1, ADC_SampleTime_15Cycles ); // PC0
  // ADC2 Regular Channel 11
  ADC_RegularChannelConfig( ADC2, ADC_Channel_11, 1, ADC_SampleTime_15Cycles ); // PC1
  ADC_MultiModeDMARequestAfterLastTransferCmd( ENABLE );
  ADC_Cmd( ADC1, ENABLE );
  ADC_Cmd( ADC2, ENABLE ); 
}


/**
  * @brief  Delay Function.
  * @param  nCount:specifies the Delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
