/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "sdio_sd.h"
#include "sdio.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);

/* Main ----------------------------------------------------------------------*/
int main(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOD, ENABLE );

  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init( GPIOD, &GPIO_InitStructure );

  if( SD_Init() != SD_OK ) {
    GPIO_SetBits( GPIOD, GPIO_Pin_13 );
    return;
  }

  // SDIO Interrupt Enable
  NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  int i;
  uint16_t outBuf[2048];
  for(i=0; i<2048; i++)
    outBuf[i] = 0x8899;
  GPIO_SetBits( GPIOD, GPIO_Pin_13 );
  SD_WriteMultiBlocks( outBuf, 0, 512, 4 );
  SD_WaitWriteOperation();
  while( SD_GetStatus() != SD_TRANSFER_OK ); 
  GPIO_ResetBits( GPIOD, GPIO_Pin_13 );
  
  GPIO_SetBits( GPIOD, GPIO_Pin_13 );
  SD_WriteMultiBlocks( outBuf, 2048, 512, 4 );
  SD_WaitWriteOperation();
  while( SD_GetStatus() != SD_TRANSFER_OK );
  GPIO_ResetBits( GPIOD, GPIO_Pin_13 );

  while(1) {
    GPIO_ToggleBits( GPIOD, GPIO_Pin_15 );
    Delay( 0xfffff );
  }
}

void SDIO_IRQHandler( void ) {
  SD_ProcessIRQSrc();
}

/* Private functions ---------------------------------------------------------*/

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
