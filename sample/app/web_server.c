#include "web_server.h"
 			
#define PSTR(s) s	


// mac address and ip address must be unique within a local area network 
static unsigned char mymac[6] = {0x55,0x55,0x58,0x10,0x00,0x25};
static unsigned char myip[4] = {192,168,3,116};

// ip address（DNS name，if you have），ip address must be“/” end this
static char baseurl[]="http://192.168.3.116/";

// tcp/www port number，rang:1-254 
static unsigned int mywwwport =80; 

// udp port number，Local development board port number 
static unsigned int myudpport =1200; 

// Send date cache 
#define BUFFER_SIZE 1500
static unsigned char buf[BUFFER_SIZE+1];

// Password，Can not more than 9 chacacters.
static char password[]="123456"; 


/*****************************************************************************
 Function    : verify_password
 Description : Comfirm password
 Input       : str
 Output      : None
 Return      : 1  password correnct
               0 password error
 *****************************************************************************/
unsigned char verify_password(char *str)
{
    // the first characters of the received string are
    // a simple password/cookie:
    if (strncmp(password,str,5)==0)
    {
        return(1);
    }
    return(0);
}

/*****************************************************************************
 Function    : analyse_get_url
 Description : takes a string of the form password/commandNumber and analyse it
 Input       : str
 Output      : -1 invalid password, otherwise command number
               -2 no command given but password valid
 Return      : None
 *****************************************************************************/
unsigned char analyse_get_url(char *str)
{
    unsigned char i=0;
    if (verify_password(str)==0)
    {
        return(-1);
    }
    // find first "/"
    // password not longer than 9 char:
    while(*str && i<10 && *str >',' && *str<'{')
    {
        if (*str=='/')
        {
            str++;
            break;
        }
        i++;
        str++;
    }
    if (*str < 0x3a && *str > 0x2f)
    {
        // is a ASCII number, return it
        return(*str-0x30);
    }
    return(-2);
}

/*****************************************************************************
 Function    : print_webpage
 Description : Send the data to the TCP send buffer
 Input       :*buf
              *on_off
 Output      : None
 Return      : plen
 *****************************************************************************/
unsigned int print_webpage(unsigned char *buf,unsigned char on_off)
{
    unsigned int plen;
    
    //Set up a new webpage 
    plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"));
	
    // Fill in the information in the middle of the page
    plen=fill_tcp_data_p(buf,plen,PSTR("<center><p> LED状态指示: "));

    if (on_off)
    {
        plen=fill_tcp_data_p(buf,plen,PSTR("<font color=\"#00FF00\"> 亮</font>"));
    }
    else
    {
        plen=fill_tcp_data_p(buf,plen,PSTR("灭"));
    }

    plen=fill_tcp_data_p(buf,plen,PSTR(" <small><a href=\""));
    plen=fill_tcp_data(buf,plen,baseurl);		 //Enter the url in the browser 
    plen=fill_tcp_data(buf,plen,password);	         // The password must be followde
    plen=fill_tcp_data_p(buf,plen,PSTR("\">[刷新]</a></small></p>\n<p><a href=\""));// the url looks like this http://baseurl/password/command
    plen=fill_tcp_data(buf,plen,baseurl);
    plen=fill_tcp_data(buf,plen,password);
    if (on_off)
    {
        plen=fill_tcp_data_p(buf,plen,PSTR("/0\">关闭LED</a><p>"));
    }
    else
    {
        plen=fill_tcp_data_p(buf,plen,PSTR("/1\">点亮LED</a><p>"));
    }
    plen=fill_tcp_data_p(buf,plen,PSTR("</center><hr><br>****************^_^ 欢迎您使用STM32F411开发板^_^****************\n"));
    
    return(plen);
}

/*****************************************************************************
 Function    : Web_Server
 Description : In the browser to create a web server, through the web inside the
               command to control the development board above the LED light off.
 Input       : None
 Output      : None
 Return      : 0 success
 Application : 1 Enter "ping 192.168.3.116" in the cmd
               2 Enter http://192.168.0.15/123456  in the browser
 *****************************************************************************/
int Web_Server(void)
{   
    unsigned int plen, i1 = 0;
    unsigned int dat_p;
    unsigned char i = 0;
    unsigned char cmd, *buf1;
    unsigned int payloadlen = 0;

    USART2_Config();
    printf("UASRT Init!\r\n");

    LED_Config();
    printf("LED Init!\r\n");

    SPI_Config();
    printf("SPI Init!\r\n");
  
    //Initialize the mac address of ENC28J60,this function must be called once. 
    enc28j60Init(mymac);

    // PHY LED configration，LED can be used to indicate the status of the communication	
    enc28j60PhyWrite(PHLCON,0x476);	
	
    //Initialize Ethernet IP layer
    init_ip_arp_udp_tcp(mymac,myip,mywwwport);
    while(1)
    {   
        // get the next new packet:			
        plen = enc28j60PacketReceive(BUFFER_SIZE, buf);      
       
        // plen will ne unequal to zero if there is a valid packet (without crc error)			
        if(plen==0)
        {
            continue;
	    GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);
        }

        // arp is broadcast if unknown but a host may also
        // verify the mac address by sending it to 
        // a unicast address.		     
        if(eth_type_is_arp_and_my_ip(buf,plen))
        {
            make_arp_answer_from_request(buf);          
            continue;
        }
      
        // check if ip packets are for us:			
        if(eth_type_is_ip_and_my_ip(buf,plen)==0) 
        {
            continue;
        }      
      
        if(buf[IP_PROTO_P]==IP_PROTO_ICMP_V && buf[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V)
        {
            // a ping packet, let's send pong  DOS 下的 ping 命令包	 
            make_echo_reply_from_request(buf, plen);          
            continue;
        }

/*-----------------tcp port www start, compare only the lower byte-----------------------------------*/
        if (buf[IP_PROTO_P]==IP_PROTO_TCP_V&&buf[TCP_DST_PORT_H_P]==0&&buf[TCP_DST_PORT_L_P]==mywwwport)
        {
            if (buf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V)
            {
                make_tcp_synack_from_syn(buf);
                // make_tcp_synack_from_syn does already send the syn,ack
                continue;
            }
            if (buf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V)
            {
                init_len_info(buf); // init some data structures
                // we can possibly have no data, just ack:
                dat_p=get_tcp_data_pointer();
                if (dat_p==0)
                {
                    if (buf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V)
                    {
                        // finack, answer with ack
                        make_tcp_ack_from_any(buf);
                    }
                    // just an ack with no data, wait for next packet
                    continue;
                }
                if (strncmp("GET ",(char *)&(buf[dat_p]),4)!=0)
                {
                    plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 OK</h1>"));
                    goto SENDTCP;
                }
                if (strncmp("/ ",(char *)&(buf[dat_p+4]),2)==0)
                {
                    plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
                    plen=fill_tcp_data_p(buf,plen,PSTR("<p>Usage: "));
                    plen=fill_tcp_data(buf,plen,baseurl);
                    plen=fill_tcp_data_p(buf,plen,PSTR("password</p>"));
                    goto SENDTCP;
                }
                cmd=analyse_get_url((char *)&(buf[dat_p+5]));

                if (cmd==-1)
                {
                    plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 401 Unauthorized\r\nContent-Type: text/html\r\n\r\n<h1>401 Unauthorized</h1>"));
                    goto SENDTCP;
                }
                if (cmd==1)	                                        
                {			
                    LED1(ON);		  
                    i=1;							
                }
                if (cmd==0)						
                {									
                    LED1(OFF);
                    i=0;							
                }
                // if (cmd==-2) or any other value
                // just display the status:
                plen=print_webpage(buf,(i));
              
                SENDTCP:
                make_tcp_ack_from_any(buf);       // send ack for http get
                make_tcp_ack_with_data(buf,plen); // send data
                continue;
            }
      }
/*-------------------------------------- tcp port www end ---------------------------------------*/

/*--------------------- udp start, we listen on udp port 1200=0x4B0 -----------------------------*/
      if (buf[IP_PROTO_P]==IP_PROTO_UDP_V&&buf[UDP_DST_PORT_H_P]==4&&buf[UDP_DST_PORT_L_P]==0xb0)
      {
          payloadlen=	  buf[UDP_LEN_H_P];
          payloadlen=payloadlen<<8;
          payloadlen=(payloadlen+buf[UDP_LEN_L_P])-UDP_HEADER_LEN;
          //payloadlen=buf[UDP_LEN_L_P]-UDP_HEADER_LEN;
          
          ANSWER:     
          for(i1=0; i1<payloadlen; i1++) buf1[i1]=buf[UDP_DATA_P+i1];      
          make_udp_reply_from_request(buf,buf1,payloadlen,myudpport);          
        
      }
/*----------------------------------------udp end -----------------------------------------------*/
    }
}

