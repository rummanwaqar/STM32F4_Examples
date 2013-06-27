/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private typedef -----------------------------------------------------------*/
GPIO_InitTypeDef  GPIO_InitStructure;

/* Private define ------------------------------------------------------------*/
#define SCK GPIO_Pin_5
#define MISO GPIO_Pin_6
#define MOSI GPIO_Pin_7

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);
void spi_init( void );
uint8_t spi_send( uint8_t );
/* Private functions ---------------------------------------------------------*/

int main(void)
{
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

  /* Configure PD12, PD13, PD14 and PD15 in output pushpull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  uint8_t rec;
  spi_init();

  while (1)
  {
    GPIO_ToggleBits( GPIOD, GPIO_Pin_15 );
    GPIOC->BSRRH |= GPIO_Pin_4;
    rec = spi_send( 0xaa );

    GPIOC->BSRRL |= GPIO_Pin_4;
    Delay( 0xffff );
  }
}

void spi_init() {
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef SPI_InitStructure;
  
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE );

  GPIO_InitStructure.GPIO_Pin = SCK | MISO | MOSI;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( GPIOA, &GPIO_InitStructure );
  
  GPIO_PinAFConfig( GPIOA, GPIO_PinSource5, GPIO_AF_SPI1 );
  GPIO_PinAFConfig( GPIOA, GPIO_PinSource6, GPIO_AF_SPI1 );
  GPIO_PinAFConfig( GPIOA, GPIO_PinSource7, GPIO_AF_SPI1 );

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init( GPIOC, &GPIO_InitStructure );

  // set SS high
  GPIOC->BSRRL |= GPIO_Pin_4;

  RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, ENABLE );

  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_Init( SPI1, &SPI_InitStructure );

  SPI_Cmd( SPI1, ENABLE );
}

uint8_t spi_send( uint8_t data ) {
  SPI1->DR = data;
  while( !( SPI1->SR & SPI_I2S_FLAG_TXE ) );
  while( !( SPI1->SR & SPI_I2S_FLAG_RXNE ) );
  while( SPI1->SR & SPI_I2S_FLAG_BSY );
  return SPI1->DR;
}

void Delay(__IO uint32_t nCount)
{
  while(nCount--)
  {
  }
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{ 
  while (1)
  {
  }
}
#endif
