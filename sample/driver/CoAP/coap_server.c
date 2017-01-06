

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "coap_server.h"
uint8_t buf[1024];
char    buff[1024];
uint8_t scratch_raw[1024];

/*****************************************************************************
 Function    : Coap_Server
 Description : Config the interrupt  handler of USART1
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
void Coap_Server()
{   
	int rc;
  coap_packet_t pkt;
  char *pStr;
	
  coap_rw_buffer_t scratch_buf = {scratch_raw, sizeof(scratch_raw)}; 
     
			
        printf("等待接受数据\r\n");
			  pStr=ESP8266_ReceiveString ( DISABLE );
	      pStr=strstr(pStr,":");
			  strcpy(buff,pStr+1);
	      printf("----------Received Buffer----------------\r\n");
	      
        if (0 != (rc = coap_parse(&pkt,(const uint8_t*)buff,strlen(buff))))        //prase the received "buff" to "pkt"
        {
            printf("Bad packet rc=%d\r\n", rc);
        }
        else
        {
            size_t rsplen = sizeof(buff)+1;                                         
            coap_packet_t rsppkt;
            coap_handle_req(&scratch_buf, &pkt, &rsppkt);                  //handle the prased  pkt then make response pkt

            if (0 != (rc = coap_build((uint8_t*)buff, &rsplen, &rsppkt)))
            {
                 printf("coap_build failed rc=%d\n", rc);
            }
            else
            {  
               printf("----------Send Buffer----------------\r\n");							
							
		coap_dump((uint8_t*)buff, rsplen, true); 
		coap_parse(&pkt,(const uint8_t*)buff,rsplen);    
              ESP8266_SendString1(DISABLE,buff,rsplen,Single_ID_0 ); //send response pkt
               
            }
        }
	 
    

}


