#include "dht11_driver.h"

/*****************************************************************************
 Function    : DHT11_GPIO_Config
 Description : Configure the temperature and humidity sensor IO port
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void DHT11_GPIO_Config(void)
{		
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(DHT11_CLK, ENABLE); 

    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

    GPIO_Init(DHT11_PORT, &GPIO_InitStructure); 
}

/*****************************************************************************
 Function    : DHT11_Mode_IPU
 Description : Causes the DHT11-DATA pin to become the pull-up input mode
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
static void DHT11_Mode_IPU(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN ; 
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);	 
}

/*****************************************************************************
 Function    : DHT11_Mode_IPU
 Description : Turn the DHT11-DATA pin into push-pull output mode
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
static void DHT11_Mode_Out_PP(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
														   
    GPIO_InitStructure.GPIO_Pin = DHT11_PIN;	
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;   
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

    GPIO_Init(DHT11_PORT, &GPIO_InitStructure);	 	 
}

/*****************************************************************************
 Function    : DHT11_Mode_IPU
 Description : Read a byte from DHT11, MSB first
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
static uint8_t Read_Byte(void)
{
    uint8_t i, temp=0;

    for(i=0;i<8;i++)    
    {	 
        while(DHT11_DATA_IN()==Bit_RESET);
                
        delay_4us(20);
        if(DHT11_DATA_IN()==Bit_SET)/* x us后仍为高电平表示数据“1” */
        {

            while(DHT11_DATA_IN()==Bit_SET);

            temp|=(uint8_t)(0x01<<(7-i));  //把第7-i位置1，MSB先行 
        }
        else	 // x us后为低电平表示数据“0”
        {			   
            temp&=(uint8_t)~(0x01<<(7-i)); //把第7-i位置0，MSB先行
        }
    }
    return temp;
}

/*****************************************************************************
 Function    : DHT11_Mode_IPU
 Description : A complete data transfer for the 40bit, high-first-out
 Input       : DHT11_Data
 Output      : None
 Return      : None
 *****************************************************************************/
uint8_t Read_DHT11(DHT11_Data_TypeDef *DHT11_Data)
{  
    uint16_t count;

    DHT11_Mode_Out_PP();

    DHT11_DATA_OUT(DHT11_LOW);
    LOS_TaskDelay(20);
    
    DHT11_DATA_OUT(DHT11_HIGH); 

    delay_4us(8);

    DHT11_Mode_IPU();
 
    if(DHT11_DATA_IN()==Bit_RESET)     
    {
        count=0;
        while(DHT11_DATA_IN()==Bit_RESET)
        {
            count++;
            if(count>1000)  return 0;
            delay_4us(3);
        }    
    
        count=0;
        while(DHT11_DATA_IN()==Bit_SET)
        {
            count++;
            if(count>1000)  
            return 0;
            delay_4us(3);

        }   
        DHT11_Data->humi_int= Read_Byte();

        DHT11_Data->humi_deci= Read_Byte();

        DHT11_Data->temp_int= Read_Byte();

        DHT11_Data->temp_deci= Read_Byte();

        DHT11_Data->check_sum= Read_Byte();
                
        DHT11_Mode_Out_PP();
                
        DHT11_DATA_OUT(DHT11_HIGH);

        if(DHT11_Data->check_sum == DHT11_Data->humi_int + DHT11_Data->humi_deci + DHT11_Data->temp_int+ DHT11_Data->temp_deci)
        {
            DHT11_Data->humi_int= DHT11_Data->humi_int*25;

            DHT11_Data->humi_deci= DHT11_Data->humi_deci*25%100;

            DHT11_Data->temp_int= DHT11_Data->temp_int*25;

            DHT11_Data->temp_deci= DHT11_Data->temp_deci*25%100;

            return 1;
        }
        else 
            return 0;
    }
    else
    {		
        return 0;
    }   
}





