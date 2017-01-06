/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher 
 * Copyright: GPL V2
 *
 * IP/ARP/UDP/TCP functions
 *
 * Chip type           : ATMEGA88 with ENC28J60
 *********************************************/


/*********************************************
 * modified: 2007-08-08
 * Author  : awake
 * Copyright: GPL V2
 * http://www.icdev.com.cn/?2213/
 * Host chip: ADUC7026
**********************************************/



//@{
#ifndef IP_ARP_UDP_TCP_H
#define IP_ARP_UDP_TCP_H


#include "enc28j60_driver.h"
#include "net.h"
// you must call this function once before you use any of the other functions:
extern void init_ip_arp_udp_tcp(unsigned char *mymac,unsigned char *myip,unsigned char wwwp);
//
extern unsigned char eth_type_is_arp_and_my_ip(unsigned char *buf,unsigned int len);
extern unsigned char eth_type_is_ip_and_my_ip(unsigned char *buf,unsigned int len);
extern void make_arp_answer_from_request(unsigned char *buf);
extern void make_echo_reply_from_request(unsigned char *buf,unsigned int len);
extern void make_udp_reply_from_request(unsigned char *buf,char *data,unsigned int datalen,unsigned int port);


extern void make_tcp_synack_from_syn(unsigned char *buf);
extern void init_len_info(unsigned char *buf);
extern unsigned int get_tcp_data_pointer(void);
extern unsigned int fill_tcp_data_p(unsigned char *buf,unsigned int pos, const unsigned char *progmem_s);
extern unsigned int fill_tcp_data(unsigned char *buf,unsigned int pos, const char *s);
extern void make_tcp_ack_from_any(unsigned char *buf);
extern void make_tcp_ack_with_data(unsigned char *buf,unsigned int dlen);




#endif /* IP_ARP_UDP_TCP_H */
//@}
