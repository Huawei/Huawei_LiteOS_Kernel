#ifndef _LED_DRIVER_H
#define _LED_DRIVER_H

#include "stm32f4xx_gpio.h"
#include "gpio_config.h"


#define ON	0
#define OFF	1
#define LED1(a)	if (a)	\
	GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);\
	else		\
	GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET)
           
           
void LED_Config(void);

void LED_ON(void);

void LED_OFF(void);


#endif 