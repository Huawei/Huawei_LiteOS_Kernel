
#include "esp8266_driver.h"
 #define  COAP_DEBUG

char  rcvFlag=0;
char* pSSID="HUAWEI-SMA";
char* pPassWord="123456789";
char* pPORT="5683";
char* pIp="192.168.3.11";


struct  STRUCT_USARTx_Fram  strEsp8266_Fram_Record = { 0 };
/*****************************************************************************
 Function    : USART1_IRQHandler
 Description :USART1 interrupt handler
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void USART1_IRQHandler( void )
{	
	char ch;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		ch  = USART_ReceiveData( USART1 );
		
		if( strEsp8266_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       
		{
			  
		        strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ++ ]  = ch;

		}
	}
	 	 
	if ( USART_GetITStatus( USART1, USART_IT_IDLE ) == SET )                                         //recevie data completely
	{
                strEsp8266_Fram_Record .InfBit .FramFinishFlag = 1;
		
		ch = USART_ReceiveData( USART1 );                                                              
	
        }	
}
/*****************************************************************************
 Function    : ESP8266_Config
 Description : Config the interrupt  handler of USART1
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void ESP8266_Config( void )
{
         USART2_Config();
         printf("usart2_init\r\n");
         USART1_Config(); 
         printf("usart1_init\r\n");
	 GPIO_Config( RCC_AHB1Periph_GPIOA ,GPIO_Pin_0|GPIO_Pin_1 , GPIO_Mode_OUT , GPIO_Speed_50MHz , GPIOA) ;
	
	 GPIO_ResetBits( GPIOA, GPIO_Pin_0 );//pull down WiFi CS pin			
	 GPIO_SetBits( GPIOA, GPIO_Pin_1 );//pull up WiFi   RST pin	

	 NVIC_USART1_Config();
}
/*****************************************************************************
 Function    : ESP8266_Choose
 Description : ENABLE/DISABLE ESP8266
 Input       : enumChoose=ENABLE
               enumChoose = DISABLE
 Output      : None
 Return      : None
 *****************************************************************************/
void ESP8266_Choose ( FunctionalState enumChoose )
{
	if ( enumChoose == ENABLE )
		ESP8266_CH_HIGH_LEVEL();
	
	else
		ESP8266_CH_LOW_LEVEL();
	
}


/*****************************************************************************
 Function    : ESP8266_Rst
 Description : Reset ESP8266
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void ESP8266_Rst ( void )
{
	#if 0
	 ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500 );   	
	
	#else
	 ESP8266_RST_LOW_LEVEL();
	 delay_ms ( 500 ); 
	 ESP8266_RST_HIGH_LEVEL();
	 
	#endif

}
bool ESP8266_GetIp (  )
{
	char* cmd="AT+CIFSR";
        ESP8266_Cmd ( cmd, "STAIP",0, 2500);  
}

/*****************************************************************************
 Function    : ESP8266_AT_Test
 Description : Test ESP8266
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void ESP8266_AT_Test ( void )
{
	char* cmd="AT";
	ESP8266_RST_HIGH_LEVEL();
	delay_ms ( 1000 ); 
	while ( ! ESP8266_Cmd ( cmd, "OK", NULL, 1000) ) ESP8266_Rst ();  	

}
/*****************************************************************************
 Function    : ESP8266_Cmd
 Description : AT Command
 Input       : cmd 
               reply1
               reply2
               waittime
 Output      : None
 Return      : true/false
 *****************************************************************************/
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime)
{    
	strEsp8266_Fram_Record .InfBit .FramLength = 0;               //从新开始接收新的数据包
        char * wocai="\r\n";
        
	UART1_PutStr(cmd);
        
       
	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
		return true;
	
	delay_ms ( waittime );                 //延时
	rcvFlag=1;
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';

	PC_Usart ( "%s\r\n", strEsp8266_Fram_Record .Data_RX_BUF );
  
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) );
	
}
/*****************************************************************************
 Function    : ESP8266_Usart1
 Description : overwrite UART1_PutStr function to adapte the package message which includes null
 Input       : cmd 
               reply1
               reply2
               waittime
 Return      : true/false
 *****************************************************************************/
void  ESP8266_Usart1( char *Data, uint8_t n)
{
  uint8_t i;
  for(i=0;i<n;i++)
  {
    USART_SendData(USART1, *Data);
    while( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
    Data++;
  }
}

/*****************************************************************************
 Function    : ESP8266_Usart1
 Description : overwrite ESP8266_Cmd function to adapte the package message which includes null
 Input       : cmd,     The command to send
               reply1,  Expected response1 
               reply2,  Expected response2
               waittime,Delaytime
 Return      : true,send  cmd  successfully
               false,send cmd  failed
 *****************************************************************************/
bool ESP8266_Cmd1( char * cmd, char * reply1, char * reply2, u32 waittime,char len)
{    
	strEsp8266_Fram_Record .InfBit .FramLength = 0;               

	ESP8266_Usart1(cmd,len);
 
	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //No need response
		return true;
	
	delay_ms ( waittime );                 //delay
	rcvFlag=1;
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';

	PC_Usart ( "%s\r\n", strEsp8266_Fram_Record .Data_RX_BUF );
  
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) );
	
}

/*****************************************************************************
 Function    : ESP8266_Net_Mode_Choose
 Description : select the work mode
 Input       :  enumMode,   1:STA
                           2:AP
                           3:STA-AP
 Return      : true, select successfully
               false,select failed
 *****************************************************************************/
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
	char* cmd="AT+CWMODE=1";
	switch ( enumMode )
	{
		case STA:
			return ESP8266_Cmd ( "AT+CWMODE=1", "OK", "no change", 500); 
		
	  case AP:
		  return ESP8266_Cmd ( "AT+CWMODE=2", "OK", "no change", 500); 
		
		case STA_AP:
		  return ESP8266_Cmd ( "AT+CWMODE=3", "OK", "no change", 500 ); 
		
	  default:
		  return false;
  }
	
}


/*****************************************************************************
 Function    : ESP8266_JoinAP
 Description : Esp8266 join the external wifi
 Input       :  pSSID,   
                pPassWord
 Return      : true, select successfully
               false,select failed
 *****************************************************************************/
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"", pSSID, pPassWord );
	
	return ESP8266_Cmd ( cCmd, "OK", "GOT IP", 16000);
	
}

/*****************************************************************************
 Function    :  ESP8266_BuildAP
 Description :  Setup wifi access point 
 Input       :  pSSID,access point name
                pPassWord,access point password
                enunPsdMode,access pointmode
 Return      : true, select successfully
               false,select failed
 *****************************************************************************/
bool ESP8266_BuildAP ( char * pSSID, char * pPassWord, char * enunPsdMode )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWSAP=\"%s\",\"%s\",1,%s", pSSID, pPassWord, enunPsdMode );
	
	return ESP8266_Cmd ( cCmd, "OK", 0, 1000 );
	
}
/*****************************************************************************
 Function    :  ESP8266_Enable_MultipleId
 Description : Enable Multiple connect 
 Input       :  enumEnUnvarnishTx,1 Enable
                                  0 Disenable

 Return      : true, select successfully
               false,select failed
 *****************************************************************************/
bool ESP8266_Enable_MultipleId ( FunctionalState enumEnUnvarnishTx )
{
	char cCmd [20];
	
	sprintf ( cCmd, "AT+CIPMUX=%d", ( enumEnUnvarnishTx ? 1 : 0 ) );
	
	return ESP8266_Cmd ( cCmd, "OK", 0, 8000);
	
}
/*****************************************************************************
 Function    : ESP8266_Link_Server
 Description : WF-ESP8266 link to server
 Input       : enumE,enumTCP 
                     enumUDP
               ip, server ip address
               ComNum,server com port
               id,connect id number
 Return      : true, select successfully
               false,select failed
 *****************************************************************************/
bool ESP8266_Link_Server ( ENUM_NetPro_TypeDef enumE, char * ip, char * ComNum, ENUM_ID_NO_TypeDef id)
{
	char cStr [100] = { 0 }, cCmd [120];

  switch (  enumE )
  {
		case enumTCP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s", "TCP", ip, ComNum );
		  break;
		
		case enumUDP:
		  sprintf ( cStr, "\"%s\",\"%s\",%s,%s", "UDP", ip, ComNum ,ComNum);
		  break;
		
		default:
			break;
  }

  if ( id < 5 )
  {  
    
    sprintf ( cCmd, "AT+CIPSTART=%s,%d", cStr,id );
  }
  else
  {
    sprintf ( cCmd, "AT+CIPSTART=%s", cStr );
  }
	return ESP8266_Cmd ( cCmd, "OK", "ALREADY CONNECTED", 1000);
	
}
/*****************************************************************************
 Function    : ESP8266_SendString
 Description : WF-ESP8266 send string
 Input       : enumEnUnvarnishTx,enable Unvarnish translate
               pStr,             the string to send 
               ulStrLength,      the length of pStr
               ucId,             connect id number
             
 Return      : true, select successfully
               false,select failed
 *****************************************************************************/
bool ESP8266_SendString ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
	char cStr [20];
	bool bRet = false;
	char* cCmd="AT+CIPMODE=1";
	if ( enumEnUnvarnishTx )
	{
		
		ESP8266_Cmd ( "AT+CIPMODE=1", "OK", 0, 5000);
		cCmd="AT+CIPSEND";
		bRet = ESP8266_Cmd ( "AT+CIPSEND", "OK", ">", 5000);
		ESP8266_Cmd ( pStr, "SEND OK", 0, 5000);
		PC_Usart ( "%s", pStr );
	}
	
	else
	{
		if ( ucId < 5 )
			sprintf ( cStr, "AT+CIPSEND=%d,%d\r\n", ucId, ulStrLength + 2 );

		else
			 sprintf ( cStr, "AT+CIPSEND=%d\r\n", ulStrLength + 2 );
		
		   ESP8266_Cmd ( cStr, ">",0, 5000);
     
		   bRet = ESP8266_Cmd ( pStr, "SEND OK", 0, 5000);
       }
	
	return bRet;

}
/*****************************************************************************
 Function    : ESP8266_SendString
 Description : overwrite the ESP8266_Send function to adapt the string which includes null
 Input       : enumEnUnvarnishTx,enable Unvarnish translate
               pStr,             the string to send 
               ulStrLength,      the length of pStr
               ucId,             connect id number
             
 Return      : true, select successfully
               false,select failed
 *****************************************************************************/
bool ESP8266_SendString1 ( FunctionalState enumEnUnvarnishTx, char * pStr, u32 ulStrLength, ENUM_ID_NO_TypeDef ucId )
{
	char cStr [20];
	bool bRet = false;
	char* cCmd="AT+CIPMODE=1";
	if ( enumEnUnvarnishTx )
	{
		
		ESP8266_Cmd ( "AT+CIPMODE=1", "OK", 0, 500 );
		cCmd="AT+CIPSEND";
		bRet = ESP8266_Cmd( "AT+CIPSEND", "OK", ">", 500);
		ESP8266_Cmd ( pStr, "SEND OK", 0, 1000);
		PC_Usart ( "%s", pStr );
	}
	
	else
	{
		if ( ucId < 5 )
			sprintf ( cStr, "AT+CIPSEND=%d,%d", ucId, ulStrLength +2);

		else
			sprintf ( cStr, "AT+CIPSEND=%d", ulStrLength  );
		
		   ESP8266_Cmd ( cStr, ">",0, 50);
     
		   bRet = ESP8266_Cmd1( pStr, "SEND OK", 0,50,ulStrLength);
        }
	
	return bRet;

}
/*****************************************************************************
 Function    : ESP8266_ReceiveString
 Description : receive  string 
 Input       :enumEnUnvarnishTx,0,enable Unvarnish translate
                                1,dienable Unvarnish translate
             
 Return      : pRecStr,
 *****************************************************************************/
char * ESP8266_ReceiveString ( FunctionalState enumEnUnvarnishTx )
{
	char * pRecStr = 0;
	
	strEsp8266_Fram_Record .InfBit .FramLength = 0;
	strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;  
	while ( ! strEsp8266_Fram_Record .InfBit .FramFinishFlag );
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ] = '\0';
	
	if ( enumEnUnvarnishTx )
	{
		//if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, ">" ) )
			pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;

	}
	
	else 
	{
		//if ( strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "+IPD" ) )
			pRecStr = strEsp8266_Fram_Record .Data_RX_BUF;

	}

	return pRecStr;
	
}

/*****************************************************************************
 Function    : ESP8266_UdpConnect
 Description : setup udp connect test
 Input       :None
             
 Return      : None
 *****************************************************************************/
void ESP8266_UdpConnect ( void )
{
	char cStrInput [100] ="ni hao"; 
	char * pStrDelimiter [2], * pStr;
	u8 uc = 0;
        u32 ul = 0;
      
	ESP8266_Choose ( ENABLE );	
	ESP8266_AT_Test();
	ESP8266_Net_Mode_Choose (STA_AP);
        ESP8266_Cmd ( "AT+CWLAP", "OK", 0, 1000);
		
        do
       {
	    PC_Usart ( "\r\n正在连接wifi请稍等\r\n" );
	    delay_ms(500);
        } while ( ! ESP8266_JoinAP (pSSID, pPassWord ) );
	
	//ESP8266_Enable_MultipleId ( ENABLE );
	
	do 
	{
		PC_Usart ( "\r\n正在建立连接  手机 请稍等  \r\n" );
		delay_ms(300);
        } while ( !ESP8266_Link_Server ( enumUDP,pIp, pPORT, Multiple_ID_2));

	
          
	#ifndef COAP_DEBUG                                           //如果定义了COAP_DEBUG则不进行UDP连接测试
	while (1)
	{
		PC_Usart ( "\r\n等待数据请稍等11\r\n" );
		delay_ms(5000);
	        pStr = ESP8266_ReceiveString ( DISABLE );
		PC_Usart ( "%s\r\n", pStr );
		printf("len:%d\r\n",strlen(pStr));
		ESP8266_SendString ( DISABLE, cStrInput,strlen(cStrInput), Single_ID_0 );
	}
       #endif
}

/*****************************************************************************
 Function    : ESP8266_STA_TCP_Client
 Description : setup station model TCP connect test
 Input       :None
             
 Return      : None
 *****************************************************************************/
void ESP8266_STA_TCP_Client ( void )
{
	char cStrInput [100] = { 0 }, * pStrDelimiter [2], * pBuf, * pStr;
	u8 uc = 0;
        u32 ul = 0;
	ESP8266_Choose ( ENABLE );	
      
	ESP8266_AT_Test ();
	ESP8266_Net_Mode_Choose ( STA );
        
	ESP8266_Cmd ( "AT+CWLAP", "OK", 0, 6000);          //ap列表
		
        do
	{
		PC_Usart ( "\r\n请输入要连接的WiFi名称和密钥，输入格式为：名称字符+英文逗号+密钥字符+空格，点击发送\r\n" );

		scanf ( "%s", cStrInput );

		PC_Usart ( "\r\n稍等片刻 ……\r\n" );

		pBuf = cStrInput;
		uc = 0;
		while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
		{
			pStrDelimiter [ uc ++ ] = pStr;
			pBuf = NULL;
		} 
		
        } while ( ! ESP8266_JoinAP ( pStrDelimiter [0], pStrDelimiter [1] ) );
	
	ESP8266_Enable_MultipleId ( DISABLE );
	
	do 
	{
		PC_Usart ( "\r\n请在电脑上将网络调试助手以TCP Server连接网络，并输入电脑的IP和端口号，输入格式为：电脑IP+英文逗号+端口号+空格，点击发送\r\n" );

		scanf ( "%s", cStrInput );

		PC_Usart ( "\r\n稍等片刻 ……\r\n" );

		pBuf = cStrInput;
		uc = 0;
		while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
		{
			pStrDelimiter [ uc ++ ] = pStr;
			pBuf = NULL;
		} 
		
         }while ( !  ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Single_ID_0 ) );

        for ( uc = 0; uc < 5; uc ++ )
	{
		PC_Usart ( "\r\n请输入端口ID%d要发送的字符串，输入格式为：字符串（不含空格）+空格，点击发送\r\n", uc );

		scanf ( "%s", cStrInput );
                PC_Usart ("%s",cStrInput);
		ul = strlen ( cStrInput );

		ESP8266_SendString( DISABLE, cStrInput,ul, Single_ID_0 );
		
	}
	
	
	PC_Usart ( "\r\n请在网络调试助手发送字符串\r\n" );	
        while (1)     
	{
	  pStr = ESP8266_ReceiveString ( DISABLE );
	  PC_Usart ( "%s", pStr );
	}

}
/*****************************************************************************
 Function    : ESP8266_STA_TCP_Client
 Description : setup AP model TCP connect test
 Input       : None
             
 Return      : None
 *****************************************************************************/
void ESP8266_AP_TCP_Server ( void )
{
	char cStrInput [100] = { 0 }, * pStrDelimiter [3], * pBuf, * pStr;
	u8 uc = 0;
        u32 ul = 0;

        ESP8266_Choose ( ENABLE );

	ESP8266_AT_Test ();
	
	ESP8266_Net_Mode_Choose ( AP );


	PC_Usart ( "\r\n请输入要创建的WiFi的名称、加密方式和密钥，加密方式的编号为：\
                    \r\n0 = OPEN\
                    \r\n1 = WEP\
                    \r\n2 = WPA_PSK\
	            \r\n3 = WPA2_PSK\
                    \r\n4 = WPA_WPA2_PSK\
		    \r\n输入格式为：名称字符+英文逗号+加密方式编号+英文逗号+密钥字符+空格，点击发送\r\n" );

	scanf ( "%s", cStrInput );

	PC_Usart ( "\r\n稍等片刻 ……\r\n" );

	pBuf = cStrInput;
	uc = 0;
	while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 
	
	ESP8266_BuildAP ( pStrDelimiter [0], pStrDelimiter [2], pStrDelimiter [1] );
	ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500); //*
		

	ESP8266_Enable_MultipleId ( ENABLE );
		
	
	PC_Usart ( "\r\n请输入服务器要开启的端口号和超时时间（0~28800，单位：秒），输入格式为：端口号字符+英文逗号+超时时间字符+空格，点击发送\r\n" );

	scanf ( "%s", cStrInput );

	PC_Usart ( "\r\n稍等片刻 ……\r\n" );

	pBuf = cStrInput;
	uc = 0;
	while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 

	ESP8266_StartOrShutServer ( ENABLE, pStrDelimiter [0], pStrDelimiter [1] );
	
	
	do
	{
		PC_Usart ( "\r\n正查询本模块IP……\r\n" );
	  ESP8266_Cmd ( "AT+CIFSR", "OK", "Link", 5000);
		
		PC_Usart ( "\r\n请用手机连接刚才创建的WiFi，这里只连接一个手机，作为ID0，然后用手机网络调试助手以TCP Client连接刚才开启的服务器（AP IP）……\r\n" );
		delay_ms ( 20000 ) ;
	}	while ( ! ESP8266_Cmd ( "AT+CIPSTATUS", "+CIPSTATUS:0", 0, 5000) );
	

	PC_Usart ( "\r\n请输入要向端口手机（ID0）发送的字符串，输入格式为：字符串（不含空格）+空格，点击发送\r\n" );

	scanf ( "%s", cStrInput );

	ul = strlen ( cStrInput );
	
	ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_0 );

	
	PC_Usart ( "\r\n请在手机网络调试助手发送字符串\r\n" );
	while (1)
	{
	  pStr = ESP8266_ReceiveString ( DISABLE );
		PC_Usart ( "%s", pStr );
	}

}
 /*****************************************************************************
 Function    : ESP8266_StaTcpClient_ApTcpServer
 Description : setup AP model TCP server test AND STA model TCP client test
 Input       : None
             
 Return      : None
 *****************************************************************************/
void ESP8266_StaTcpClient_ApTcpServer ( void )
{
	char cStrInput [100] = { 0 }, * pStrDelimiter [3], * pBuf, * pStr;
	u8 uc = 0;
        u32 ul = 0;

        ESP8266_Choose ( ENABLE );

	ESP8266_AT_Test ();
	
	ESP8266_Net_Mode_Choose ( STA_AP );


	PC_Usart ( "\r\n请输入要创建的WiFi的名称、加密方式和密钥，加密方式的编号为：\
						\r\n0 = OPEN\
						\r\n1  =WEP\
						\r\n2 = WPA_PSK\
						\r\n3 = WPA2_PSK\
						\r\n4 = WPA_WPA2_PSK\
						\r\n输入格式为：名称字符+英文逗号+加密方式编号+英文逗号+密钥字符+空格，点击发送\r\n" );

	scanf ( "%s", cStrInput );

	PC_Usart ( "\r\n稍等片刻 ……\r\n" );

	pBuf = cStrInput;
	uc = 0;
	while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 
	
	ESP8266_BuildAP ( pStrDelimiter [0], pStrDelimiter [2], pStrDelimiter [1] );
	ESP8266_Cmd ( "AT+RST", "OK", "ready", 2500); //*
	

	ESP8266_Cmd ( "AT+CWLAP", "OK", 0, 5000);
		
  do
	{
		PC_Usart ( "\r\n请输入要连接的WiFi名称和密钥，输入格式为：名称字符+英文逗号+密钥字符+空格，点击发送\r\n" );

		scanf ( "%s", cStrInput );

		PC_Usart ( "\r\n稍等片刻 ……\r\n" );

		pBuf = cStrInput;
		uc = 0;
		while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
		{
			pStrDelimiter [ uc ++ ] = pStr;
			pBuf = NULL;
		} 
		
  } while ( ! ESP8266_JoinAP ( pStrDelimiter [0], pStrDelimiter [1] ) );

	
	ESP8266_Enable_MultipleId ( ENABLE );
		
	
	PC_Usart ( "\r\n请输入服务器要开启的端口号和超时时间（0~28800，单位：秒），输入格式为：端口号字符+英文逗号+超时时间字符+空格，点击发送\r\n" );

	scanf ( "%s", cStrInput );

	PC_Usart ( "\r\n稍等片刻 ……\r\n" );

	pBuf = cStrInput;
	uc = 0;
	while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
	{
		pStrDelimiter [ uc ++ ] = pStr;
		pBuf = NULL;
	} 

	ESP8266_StartOrShutServer ( ENABLE, pStrDelimiter [0], pStrDelimiter [1] );
	
	
	do 
	{
		PC_Usart ( "\r\n请在电脑上将网络调试助手以TCP Server连接网络，并输入电脑的IP和端口号，输入格式为：电脑IP+英文逗号+端口号+空格，点击发送\r\n" );

		scanf ( "%s", cStrInput );

		PC_Usart ( "\r\n稍等片刻 ……\r\n" );

		pBuf = cStrInput;
		uc = 0;
		while ( ( pStr = strtok ( pBuf, "," ) ) != NULL )
		{
			pStrDelimiter [ uc ++ ] = pStr;
			pBuf = NULL;
		} 
		
  } while ( ! ( ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_0 ) &&
	              ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_1 ) &&
	              ESP8266_Link_Server ( enumTCP, pStrDelimiter [0], pStrDelimiter [1], Multiple_ID_2 ) ) );
		
	
	do
	{
		PC_Usart ( "\r\n正查询本模块IP，前一个为AP IP，后一个为STA IP……\r\n" );
	  ESP8266_Cmd ( "AT+CIFSR", "OK", "Link", 500);
		
		PC_Usart ( "\r\n请用手机连接刚才创建的WiFi，这里只连接一个手机，作为ID3，然后用手机网络调试助手以TCP Client连接刚才开启的服务器（AP IP）……\r\n" );
		delay_ms ( 20000 ) ;
	}	while ( ! ESP8266_Cmd ( "AT+CIPSTATUS", "+CIPSTATUS:3", 0, 500) );
	

	for ( uc = 0; uc < 3; uc ++ )
	{
		PC_Usart ( "\r\n请输入端口ID%d要发送的字符串，输入格式为：字符串（不含空格）+空格，点击发送\r\n", uc );

		scanf ( "%s", cStrInput );

		ul = strlen ( cStrInput );
		
		ESP8266_SendString ( DISABLE, cStrInput, ul, ( ENUM_ID_NO_TypeDef ) uc );
		
	}
	
	
	PC_Usart ( "\r\n请输入要向端口手机（ID3）发送的字符串，输入格式为：字符串（不含空格）+空格，点击发送\r\n" );

	scanf ( "%s", cStrInput );

	ul = strlen ( cStrInput );
	
	ESP8266_SendString ( DISABLE, cStrInput, ul, Multiple_ID_3 );

	
	PC_Usart ( "\r\n请在电脑网络调试助手或手机网络调试助手发送字符串……\r\n" );
	while (1)
	{
	  pStr = ESP8266_ReceiveString ( DISABLE );
		PC_Usart ( "%s", pStr );
	}
	
}


