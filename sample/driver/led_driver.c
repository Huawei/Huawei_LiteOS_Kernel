#include "led_driver.h"
  
/*****************************************************************************
 Function    : LED_Config
 Description : Config LED
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void LED_Config(void)               
{
    GPIO_Config(RCC_AHB1Periph_GPIOA , GPIO_Pin_5 , GPIO_Mode_OUT , GPIO_Speed_50MHz , GPIOA );

}

/*****************************************************************************
 Function    : LED_ON
 Description : Turn on the led
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void LED_ON(void)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);    
}

/*****************************************************************************
 Function    : LED_OFF
 Description : Turn off the led
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void LED_OFF(void)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);
}


