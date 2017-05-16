#include "dht_test.h"


DHT11_Data_TypeDef DHT11_Data;

/*****************************************************************************
 Function    : DHT11_TEST
 Description : Temperature and humidity sensor operation
 Input       : None
 Output      : None
 Return      : None 
******************************************************************************/
int DHT11_TEST(void)
{	 

    USART2_Config(); 	

    DHT11_GPIO_Config();	
    while(1)
    {
        if( Read_DHT11 ( & DHT11_Data ) == SUCCESS)
        {             
            printf("Humi:%d.%d%RH  Temp:%d.%d °C\r\n\n\n",\
            DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
        }		
        else
        {
            printf("Read DHT11 ERROR!\r\n\n\n");
        }
        LOS_TaskDelay(2000);

    } 
}



