#include <stdbool.h>
#include <string.h>
#include "coap.h"
#include "endpoints.h"
#include "cJSON.h"
#include "stm32f4xx_gpio.h"
static char light = '0';

const uint16_t rsplen = 1500;
static char rsp[1500] = "";
void build_rsp(void);
char light_flag=0;

#ifdef ARDUINO
#include "Arduino.h"
static int led = 6;
void endpoint_setup(void)
{                
    pinMode(led, OUTPUT);     
    build_rsp();
}
#else
#include <stdio.h>
void Endpoint_Setup(void)
{
    build_rsp();
}
#endif
static const coap_endpoint_path_t path_light = {1, {"light"}};  
static const coap_endpoint_path_t path_test_json = {1, {"test_json"}};  
static const coap_endpoint_path_t path_well_known_core = {2, {".well-known", "core"}};
static const coap_endpoint_path_t path_rd = {1, {".rd"}};

static int handle_get_well_known_core(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    return coap_make_response(scratch, outpkt, (const uint8_t *)rsp, strlen(rsp), id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_LINKFORMAT);
}

static int handle_get_light(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
	
    return coap_make_response(scratch, outpkt, (const uint8_t *)&light, 1, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_TEXT_PLAIN);
	   
}

static int handle_put_light(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt, uint8_t id_hi, uint8_t id_lo)
{
    if (inpkt->payload.len == 0)
        return coap_make_response(scratch, outpkt, NULL, 0, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_BAD_REQUEST, COAP_CONTENTTYPE_TEXT_PLAIN);
    if (inpkt->payload.p[0] == '1')
    {
        light ='1';
	 light_flag=1;
	 printf("ON\r\n");
	if(light_flag==1)
       {
         GPIO_SetBits(GPIOA,GPIO_Pin_5);
       }
     
        return coap_make_response(scratch, outpkt, (const uint8_t *)&light, 1, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CHANGED, COAP_CONTENTTYPE_TEXT_PLAIN);
    }
    else
    {
        light = '0';
        GPIO_ResetBits(GPIOA,GPIO_Pin_5);
		 
        printf("OFF\r\n");
        return coap_make_response(scratch, outpkt, (const uint8_t *)&light, 1, id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CHANGED, COAP_CONTENTTYPE_TEXT_PLAIN);
    }
}
static int handle_get_test_json(coap_rw_buffer_t *scratch, 
                          const coap_packet_t *inpkt, 
                          coap_packet_t *outpkt, 
                          uint8_t id_hi, uint8_t id_lo)
{
    cJSON *root;
    char *out;
    int value = 12;
    int ret = 0;

    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "value", cJSON_CreateNumber(value));
    out = cJSON_Print(root); 
    cJSON_Delete(root); 
   
    ret = coap_make_response(scratch, outpkt, 
                             (const uint8_t *)out, strlen(out), 
                             id_hi, id_lo, 
                             &inpkt->tok, 
                             COAP_RSPCODE_CONTENT, 
                             COAP_CONTENTTYPE_APPLICATION_JSON);
    //rt_free(out);
    return ret;
}
static int handle_get_rd(coap_rw_buffer_t *scratch, 
                          const coap_packet_t *inpkt, 
                          coap_packet_t *outpkt, 
                          uint8_t id_hi, uint8_t id_lo)
{
   return coap_make_response(scratch, outpkt, (const uint8_t *)rsp, strlen(rsp), id_hi, id_lo, &inpkt->tok, COAP_RSPCODE_CONTENT, COAP_CONTENTTYPE_APPLICATION_LINKFORMAT);
}


const coap_endpoint_t endpoints[] =
{
    {COAP_METHOD_GET, handle_get_well_known_core, &path_well_known_core, "ct=40"},
		{COAP_METHOD_GET, handle_get_rd, &path_well_known_core, "ct=40"},
    {COAP_METHOD_GET, handle_get_light, &path_light, "ct=0"},
    {COAP_METHOD_PUT, handle_put_light, &path_light, NULL},    //修改最后NULL为 "ct=0"
    {COAP_METHOD_GET, handle_get_test_json, &path_test_json, "ct=50"}, 
    {(coap_method_t)0, NULL, NULL, NULL}
};

void build_rsp(void)
{
    uint16_t len = rsplen;
    const coap_endpoint_t *ep = endpoints;
    int i;

    len--; // Null-terminated string

    while(NULL != ep->handler)
    {
        if (NULL == ep->core_attr) {
            ep++;
            continue;
        }

        if (0 < strlen(rsp)) {
            strncat(rsp, ",", len);
            len--;
        }

        strncat(rsp, "<", len);
        len--;

        for (i = 0; i < ep->path->count; i++) {
            strncat(rsp, "/", len);
            len--;

            strncat(rsp, ep->path->elems[i], len);
            len -= strlen(ep->path->elems[i]);
        }

        strncat(rsp, ">;", len);
        len -= 2;

        strncat(rsp, ep->core_attr, len);
        len -= strlen(ep->core_attr);

        ep++;
    }
		printf("build--rsp:%s",rsp);
}

