#ifndef __ESP8266_DRIVER_H
#define	__ESP8266_DRIVER_H 

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#include <string.h> 
#include <stdio.h>  
#include <stdbool.h>
#include "gpio_config.h"
#include "usart_config.h"
#include "delay_driver.h"
#define     ESP8266_Usart                       UART1_PutStr   

#define     PC_Usart( fmt, ... )                printf ( fmt, ##__VA_ARGS__ ) 

#define     ESP8266_CH_HIGH_LEVEL()             GPIO_SetBits( GPIOA, GPIO_Pin_0 )
#define     ESP8266_CH_LOW_LEVEL()              GPIO_ResetBits( GPIOA, GPIO_Pin_0 )

#define     ESP8266_RST_HIGH_LEVEL()            GPIO_SetBits( GPIOA, GPIO_Pin_1 )
#define     ESP8266_RST_LOW_LEVEL()             GPIO_ResetBits( GPIOA, GPIO_Pin_1 )
#define RX_BUF_MAX_LEN     1024                                     //℅?∩車?車那??o∩?℅??迆那y


struct  STRUCT_USARTx_Fram                                  //∩??迆那y?Y??米?∩|角赤?芍11足?
{
	char  Data_RX_BUF[ RX_BUF_MAX_LEN ];
	
  union {
           __IO u16 InfAll;
    struct {
		  __IO u16 FramLength       :15;                               // 14:0 
		  __IO u16 FramFinishFlag   :1;                                // 15 
	  } InfBit;
  }; 
	
};

typedef enum{
	 STA,
         AP,
         STA_AP  
} ENUM_Net_ModeTypeDef;


typedef enum{
	 enumTCP,
	 enumUDP,
} ENUM_NetPro_TypeDef;
	

typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;
	

typedef enum{
	OPEN = 0,
	WEP = 1,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSK = 4,
} ENUM_AP_PsdMode_TypeDef;






void        ESP8266_Config();
void        ESP8266_Choose                      ( FunctionalState enumChoose );
void        ESP8266_Rst                         ( void );
void        ESP8266_AT_Test                     ( void );
void        ESP8266_UdpConnect                  ( void );
bool        ESP8266_Cmd                         ( char * cmd, char * reply1, char * reply2, u32 waittime);
bool        ESP8266_Net_Mode_Choose             ( ENUM_Net_ModeTypeDef enumMode );
bool        ESP8266_JoinAP                      ( char * pSSID, char * pPassWord );
bool        ESP8266_BuildAP                     ( char * pSSID, char * pPassWord, char * enunPsdMode );
bool        ESP8266_Enable_MultipleId           ( FunctionalState enumEnUnvarnishTx );
bool        ESP8266_Link_Server                 ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id);
bool        ESP8266_StartOrShutServer           ( FunctionalState enumMode, char * pPortNum, char * pTimeOver );
bool        ESP8266_SendString                  ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId );
bool        ESP8266_SendString1                  ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId );
char *      ESP8266_ReceiveString               ( FunctionalState enumEnUnvarnishTx );

void        ESP8266_STA_TCP_Client              ( void );
void        ESP8266_AP_TCP_Server               ( void );
void        ESP8266_StaTcpClient_ApTcpServer    ( void );

extern struct  STRUCT_USARTx_Fram  strEsp8266_Fram_Record;
extern void USART1_IRQHandler();
#endif    /* __WIFI_FUNCTION_H */





