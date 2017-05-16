/*
*		 
*USART2(DEBUG)：         PA2(TX)
*                      PA3(RX) 
*
*
*stm32f411_pin	       (GND----------------PA9)
*                       |                   |                                              
*esp8266(heads-up)     GND  GPIO2  GPIO0  URXD
*                      UTXD CH_PD  RST    VCC
*                       |     |     |      |
* stm32f411_pin       (PA10--PA0---PA1----3.3V)
*
Note：esp8266 power question，Insufficient power supply may cause garbled or without data。
*/

#include "esp8266_test.h"


#define  COAP_DEBUG

void ( * pNet_Test ) ( void );


void ESP8266_Test(void)
{
   char cCh;	
   

        ESP8266_Config();
        
#ifdef  COAP_DEBUG  
        GPIO_Config(RCC_AHB1Periph_GPIOA , GPIO_Pin_5 , GPIO_Mode_OUT , GPIO_Speed_50MHz , GPIOA );
        Endpoint_Setup();  //setup endpoints    
        ESP8266_UdpConnect();
	while(1)
	{
	 
          Coap_Server();
        }		
#endif    
       
	PC_Usart("\r\n WF-ESP8266 WiFi module test Demo\r\n");                            //Guidance information

        PC_Usart ( "\r\nplease input the test mode number as follow：\
                    \r\n1.STA mode： TCP Client\
                    \r\n2.AP  mode： AP Server\
                    \r\n3.STA+AP mode： (STA)TCP Client + (AP)TCP Server\
                     \r\ninput template as：number+space， click send\r\n" );
	
        scanf ( "%c",  & cCh );

        switch ( cCh )
        {
	   case '1':
		   
		    PC_Usart("\r\nthe test mode is ：1.STA： TCP Client\r\n"); 
                    ESP8266_STA_TCP_Client ( );
		    break;
		
	   case '2':
		    PC_Usart("\r\nthe test mode is：2.AP： TCP Server\r\n");
                    ESP8266_AP_TCP_Server();
		    break;

	   case '3':
		    PC_Usart("\r\nthe test mode is：3.STA+AP： (STA)TCP Client + (AP)TCP Server\r\n");
                    ESP8266_StaTcpClient_ApTcpServer();
		    break;
           case '4':
                    PC_Usart("\r\nthe test mode is：ESP8266_UdpConnect：\r\n");
                    ESP8266_UdpConnect();
	   default:
		   pNet_Test = NULL;
		   PC_Usart("\r\ninput Error，it will finish！\r\n");
		   break;		
			
	}
    	
}
                   
