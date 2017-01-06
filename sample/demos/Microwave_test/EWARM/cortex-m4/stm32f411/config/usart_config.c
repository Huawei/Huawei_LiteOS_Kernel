#include "usart_config.h"

/*****************************************************************************
Function :NVIC_USART1_Config
Description : USART1 interrupt Initialization
Input : None
Output : None
Return : None 
******************************************************************************/
void NVIC_USART1_Config(void)
{
        NVIC_InitTypeDef NVIC_InitStructure;
        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
        
        NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x00;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02;
        NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
        NVIC_Init(&NVIC_InitStructure);
}

/*****************************************************************************
Function :USART1_Config
Description : USART1 GPIO Initialization
Input : None
Output : None
Return : None 
******************************************************************************/
void USART1_Config(void)
{
        GPIO_InitTypeDef GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
        
        
        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
   
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1); 
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOA, &GPIO_InitStructure);            
                        
        

        USART_InitStructure.USART_BaudRate=115200;
        USART_InitStructure.USART_WordLength=USART_WordLength_8b;
        USART_InitStructure.USART_StopBits=USART_StopBits_1;
        USART_InitStructure.USART_Parity=USART_Parity_No;
        USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
        USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
        
        USART_Init(USART1,&USART_InitStructure);
        
        USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
        
        USART_Cmd(USART1,ENABLE); 
       
}

/*****************************************************************************
Function :USART2_Config
Description :USART2 GPIO Initialization
Input : None
Output : None
Return : None 
******************************************************************************/
void USART2_Config( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
   
	/* config USART2 clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);  
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* USART2 mode config */
	USART_InitStructure.USART_BaudRate = 115200;               
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	

	USART_Init(USART2, &USART_InitStructure); 
 
	USART_Cmd(USART2, ENABLE);
       
	
}


//Remap printf to USARTx
int fputc(int ch,FILE *f)
{
   
                
    /* wait until send completely */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);                
     /* send a  byte to USART1 */
    USART_SendData(USARTx, (uint8_t) ch);    
    return (ch);
}

///Remap printf to USARTx
int fgetc(FILE *f)
{
    /* wait for input data  */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET);

    return (int)USART_ReceiveData(USARTx);
}
/*********************************************END OF FILE**********************/