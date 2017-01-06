#ifndef I2C_CONFIG_H
#define I2C_CONFIG_H
#include "stm32f4xx.h"
#include "stdbool.h"
/*-------------------------------------------
	Slaver Address 7bit!!!
-------------------------------------------*/

#define SCL_H         GPIOB->BSRRL = GPIO_Pin_4 /* GPIO_SetBits(GPIOB , GPIO_Pin_10)   */
#define SCL_L         GPIOB->BSRRH  = GPIO_Pin_4 /* GPIO_ResetBits(GPIOB , GPIO_Pin_10) */

#define SDA_H         GPIOB->BSRRL = GPIO_Pin_5 /* GPIO_SetBits(GPIOB , GPIO_Pin_11)   */
#define SDA_L         GPIOB->BSRRH  = GPIO_Pin_5 /* GPIO_ResetBits(GPIOB , GPIO_Pin_11) */

#define SCL_read      /*GPIOA->IDR  & GPIO_Pin_6*/ GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_4)
#define SDA_read      /*GPIOA->IDR  & GPIO_Pin_7*/ GPIO_ReadInputDataBit(GPIOB , GPIO_Pin_5)

#define SCL_TIME_OUT 20


 void I2C_Initialize(void);
	
 bool I2C_WriteBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data);
 bool I2C_ReadBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
	
	 bool I2C_ReadByte(uint8_t devAddr,uint8_t regAddr,uint8_t *data);
	 bool I2C_WriteByte(uint8_t devAddr,uint8_t regAddr,uint8_t data);
	
	 bool I2C_WriteBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);

	 void I2C_Delay(void);
	 bool I2C_Start(void);
	 void I2C_Stop(void);
	 void I2C_Ack(void);
	 void I2C_NoAck(void);
	 bool I2C_WaitAck(void);
	 void I2C_SendByte(uint8_t byte);
	 uint8_t I2C_ReceiveByte(void);



#endif
