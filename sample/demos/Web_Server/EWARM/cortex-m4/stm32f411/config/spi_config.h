#ifndef __SPI_CONFIG_H
#define __SPI_CONFIG_H
//TODO #ifdef m4
#include "stm32f4xx.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"


#define  GPIO_PIN   GPIO_Pin_12
#define  SPI_GPIO  GPIOB
#define  SPIx  SPI2
void SPI_Config(void);
unsigned char	SPI_ReadWrite(unsigned char writedat);


#endif /* __SPI_ENC28J60_H */
