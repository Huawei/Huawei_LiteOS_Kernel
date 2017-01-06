#ifndef __GPIO_CONFIG_H
#define __GPIO_CONFIG_H
//TODO #ifdef m4
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

void GPIO_Config(uint32_t RCC_AHB1Periph_GPIOx , uint32_t GPIO_Pin_x,GPIOMode_TypeDef GPIO_Mode , GPIOSpeed_TypeDef  GPIO_Speed ,GPIO_TypeDef* GPIOx);



#endif /* __GPIO_CONFIG_H */
