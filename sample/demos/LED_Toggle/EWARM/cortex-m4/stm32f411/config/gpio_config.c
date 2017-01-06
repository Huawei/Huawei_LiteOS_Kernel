
#include "gpio_config.h"


/*****************************************************************************
 Function    : GPIO_Config
 Description : Configuration GPIO
 Input       : RCC_AHB1Periph_GPIOx
               GPIO_Pin_x
               GPIO_Mode
               GPIO_Speed
               GPIOx
 Output      : None
 Return      : None
 *****************************************************************************/
void GPIO_Config(uint32_t RCC_AHB1Periph_GPIOx , uint32_t GPIO_Pin_x ,GPIOMode_TypeDef GPIO_Mode , GPIOSpeed_TypeDef  GPIO_Speed, GPIO_TypeDef* GPIOx ) //
{
    GPIO_InitTypeDef GPIO_InitStructure;                     //Declarte GPIO Init structure for init GPIO
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx,ENABLE)     ;//Enable the AHB1 peripheral clock.

    GPIO_InitStructure.GPIO_Pin= (GPIO_Pin_x);               //Specifies the GPIO pins to be configured.
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode;	             //Specifies the operating mode for the selected pins.
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed;	             //Specifies the speed for the selected pins
    GPIO_Init(GPIOx,&GPIO_InitStructure); 	             //Initializes the GPIOx peripheral according to the specified parameters in the GPIO_InitStruct.

}