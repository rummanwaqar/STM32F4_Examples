/**
********************************************************************************

********************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include <stdio.h>
#include "usbd_cdc_vcp.h"

/* Private macro */
/* Private variables */
uint32_t button_sts;
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
uint32_t i = 0;
/* Private function prototypes */
/* Private functions */

/**
**===========================================================================
**  Main
**===========================================================================
*/

int main(void)
{

	SystemInit();

	GPIO_InitTypeDef GPIO_InitStructure;
	
	// Init LEDs
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// Init Button
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Init USB
	USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_CDC_cb,&USR_cb);

	while (1){
		if(usb_cdc_kbhit()){
			char c, buffer_out[15];
			c = usb_cdc_getc();
			switch(c){
				case '3':
					GPIO_ToggleBits(GPIOD, GPIO_Pin_12);
					sprintf(buffer_out,"LED%c = %u\r\n",c,GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_12));
					usb_cdc_printf(buffer_out);
					break;
				case '4':
					GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
					sprintf(buffer_out,"LED%c = %u\r\n",c,GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13));
					usb_cdc_printf(buffer_out);
					break;
				case '5':
					GPIO_ToggleBits(GPIOD, GPIO_Pin_14);
					sprintf(buffer_out,"LED%c = %u\r\n",c,GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_14));
					usb_cdc_printf(buffer_out);
					break;
				case '6':
					GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
					sprintf(buffer_out,"LED%c = %u\r\n",c,GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_15));
					usb_cdc_printf(buffer_out);
					break;
			}
		}

		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)){
			GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
		}
	}
}
