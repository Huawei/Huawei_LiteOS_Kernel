
#include "gpio_config.h"


/*****************************************************************************
 Function    : GPIO_Config
 Description : GPIO config
 Input       : RCC_AHB1Periph_GPIOx
               GPIO_Pin_x
               GPIO_Mode
               GPIO_Speed
               GPIOx
 Output      : None
 Return      : None 
******************************************************************************/
void GPIO_Config(uint32_t RCC_AHB1Periph_GPIOx , uint32_t GPIO_Pin_x ,GPIOMode_TypeDef GPIO_Mode , GPIOSpeed_TypeDef  GPIO_Speed, GPIO_TypeDef* GPIOx ) //
{
    /*----------------------------------------------------------------------------------*/
    GPIO_InitTypeDef GPIO_InitStructure; 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOx,ENABLE);

   
    GPIO_InitStructure.GPIO_Pin= (GPIO_Pin_x); 
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode;	 
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed;	 
    GPIO_Init(GPIOx,&GPIO_InitStructure); 	  

}