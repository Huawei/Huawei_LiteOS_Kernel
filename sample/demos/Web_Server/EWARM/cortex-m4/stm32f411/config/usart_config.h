#ifndef __USART_CONFIG_H_
#define __USART_CONFIG_H_

#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"
#include <stdio.h>


#define USARTx  USART2

void NVIC_USART1_Config(void);
void USART1_Config(void);
void USART2_Config(void);
#endif