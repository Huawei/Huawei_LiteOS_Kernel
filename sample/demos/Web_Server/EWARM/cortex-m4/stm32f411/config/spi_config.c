
#include "spi_config.h"

/*****************************************************************************
 Function    : SPI_Config
 Description : Config SPI of ENC28J60
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/

void SPI_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;	
	
    // Config USART1 clock 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 , ENABLE);
	
    // Changes the mapping of the specified pin.
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);  
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource12,GPIO_AF_SPI2);
    
    
    /* Initializes the communication GPIO for enc28j60
    * PB13-SPI2-SCK :ENC28J60_SCK
    * PB14-SPI2-MISO:ENC28J60_SO
    * Pb15-SPI2-MOSI:ENC28J60_SI	 
    */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  
   
    /* Initializes the control GPIO for enc28j60
    * PB12-SPI2-NSS:ENC28J60_CS */ 					
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	   
    GPIO_Init(GPIOB, &GPIO_InitStructure);						  
    GPIO_SetBits(GPIOB, GPIO_Pin_12);										 

 

    // Config SPI2 mode 
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStructure);
	
    // Enable SPI2 
    SPI_Cmd(SPI2, ENABLE); 
}

/*****************************************************************************
 Function    : SPI_ReadWrite
 Description : SPI read or write one byte
 Input       : writedat
 Output      : None
 Return      : None
 *****************************************************************************/

unsigned char	SPI_ReadWrite(unsigned char writedat)
{
    
    // Loop while DR register in not emplty 
    while(SPI_I2S_GetFlagStatus(SPIx,SPI_I2S_FLAG_TXE) == RESET);
	
    // Send byte through the SPIx peripheral 
    SPI_I2S_SendData(SPIx, writedat);
	
    // Wait to receive a byte 
    while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
	
    // Return the byte read from the SPIx bus 
    return SPI_I2S_ReceiveData(SPIx);
}
