#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include "coap.h"

extern void endpoint_setup(void);
extern const coap_endpoint_t endpoints[];
#define DEBUG
#ifdef DEBUG
void coap_dumpHeader(coap_header_t *hdr)
{
    printf("Header:\n");
    printf("  ver  0x%02X\r\n", hdr->ver);
    printf("  t    0x%02X\r\n", hdr->t);
    printf("  tkl  0x%02X\r\n", hdr->tkl);
    printf("  code 0x%02X\r\n", hdr->code);
    printf("  id   0x%02X%02X\r\n", hdr->id[0], hdr->id[1]);
}
void coap_dumpToken(coap_buffer_t *tok)
{

    printf("Token:\r\n");
    printf("  token  0x%02X\r\n", *(tok->p));
    printf("  len    0x%02X\r\n", tok->len);


}
void coap_dumpPayload(coap_buffer_t *Payload)
{

    printf("Payload:\r\n");
    printf("  Payload  0x%02X\r\n", *(Payload->p));

}
void coap_dumpOptions(coap_option_t *opts, size_t numopt)
{
    size_t i;
    printf("Options:\n");
    for (i=0;i<numopt;i++)
    {
        printf("  0x%02X [ ", opts[i].num);
        coap_dump(opts[i].buf.p, opts[i].buf.len, true);
        printf(" ]\r\n");
    }
}
void coap_dump(const uint8_t *buf, size_t buflen, bool bare)
{
    if (bare)
    {
        while(buflen--)
            printf("%02x%s", *buf++, (buflen > 0) ? " " : "");
    }
    else
    {
        printf("Dump: \r\n");
        while(buflen--)
            printf("%02X%s", *buf++, (buflen > 0) ? " " : "");
        printf("\r\n");
    }
}
void coap_dumpPacket(coap_packet_t *pkt)
{
    coap_dumpHeader(&pkt->hdr);
    coap_dumpOptions(pkt->opts, pkt->numopts);
    printf("Payload: ");
    coap_dump(pkt->payload.p, pkt->payload.len, true);
    printf("\r\n");
}
#endif

int coap_parseHeader(coap_header_t *hdr, const uint8_t *buf, size_t buflen)
{
    if (buflen < 4)
        return COAP_ERR_HEADER_TOO_SHORT;
    hdr->ver = (buf[0] & 0xC0) >> 6;
    if (hdr->ver != 1)
        return COAP_ERR_VERSION_NOT_1;
    hdr->t = (buf[0] & 0x30) >> 4;
    hdr->tkl = buf[0] & 0x0F;
    hdr->code = buf[1];
    hdr->id[0] = buf[2];
    hdr->id[1] = buf[3];
    return 0;
}

int coap_parseToken(coap_buffer_t *tokbuf, const coap_header_t *hdr, const uint8_t *buf, size_t buflen)
{
    if (hdr->tkl == 0)
    {
        tokbuf->p = NULL;
        tokbuf->len = 0;
        return 0;
    }
    else
    if (hdr->tkl <= 8)
    {
        if (4U + hdr->tkl > buflen)
            return COAP_ERR_TOKEN_TOO_SHORT;   // tok bigger than packet
        tokbuf->p = buf+4;  // past header
        tokbuf->len = hdr->tkl;
        return 0;
    }
    else
    {
        // invalid size
        return COAP_ERR_TOKEN_TOO_SHORT;
    }
}

// advances p
int coap_parseOption(coap_option_t *option, uint16_t *running_delta, const uint8_t **buf, size_t buflen)
{
    const uint8_t *p = *buf;
    uint8_t headlen = 1;
    uint16_t len, delta;

    if (buflen < headlen) // too small
        return COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER;

    delta = (p[0] & 0xF0) >> 4;
    len = p[0] & 0x0F;

    // These are untested and may be buggy
    if (delta == 13)
    {
        headlen++;
        if (buflen < headlen)
            return COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER;
        delta = p[1] + 13;
        p++;
    }
    else
    if (delta == 14)
    {
        headlen += 2;
        if (buflen < headlen)
            return COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER;
        delta = ((p[1] << 8) | p[2]) + 269;
        p+=2;
    }
    else
    if (delta == 15)
        return COAP_ERR_OPTION_DELTA_INVALID;

    if (len == 13)
    {
        headlen++;
        if (buflen < headlen)
            return COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER;
        len = p[1] + 13;
        p++;
    }
    else
    if (len == 14)
    {
        headlen += 2;
        if (buflen < headlen)
            return COAP_ERR_OPTION_TOO_SHORT_FOR_HEADER;
        len = ((p[1] << 8) | p[2]) + 269;
        p+=2;
    }
    else
    if (len == 15)
        return COAP_ERR_OPTION_LEN_INVALID;

    if ((p + 1 + len) > (*buf + buflen))
        return COAP_ERR_OPTION_TOO_BIG;

    //printf("option num=%d\n", delta + *running_delta);
    option->num = delta + *running_delta;
    option->buf.p = p+1;
    option->buf.len = len;
    //coap_dump(p+1, len, false);

    // advance buf
    *buf = p + 1 + len;
    *running_delta += delta;

    return 0;
}

// http://tools.ietf.org/html/rfc7252#section-3.1
int coap_parseOptionsAndPayload(coap_option_t *options, uint8_t *numOptions, coap_buffer_t *payload, const coap_header_t *hdr, const uint8_t *buf, size_t buflen)
{
    size_t optionIndex = 0;
    uint16_t delta = 0;
    const uint8_t *p = buf + 4 + hdr->tkl;   // 指向options
    const uint8_t *end = buf + buflen;
    int rc;
    if (p > end)
        return COAP_ERR_OPTION_OVERRUNS_PACKET;   // out of bounds

    //coap_dump(p, end - p);

    // 0xFF is payload marker
    while((optionIndex < *numOptions) && (p < end) && (*p != 0xFF))
    {
        if (0 != (rc = coap_parseOption(&options[optionIndex], &delta, &p, end-p)))
            return rc;
        optionIndex++;
    }
    *numOptions = optionIndex;

    if (p+1 < end && *p == 0xFF)  // payload marker
    {
        payload->p = p+1;
        payload->len = end-(p+1);
    }
    else
    {
        payload->p = NULL;
        payload->len = 0;
    }

    return 0;
}


int coap_parse(coap_packet_t *pkt, const uint8_t *buf, size_t buflen)
{
    int rc;

    coap_dump(buf, buflen, false);
    if (0 != (rc = coap_parseHeader(&pkt->hdr, buf, buflen)))
        return rc;
    coap_dumpHeader(&pkt->hdr);
    if (0 != (rc = coap_parseToken(&pkt->tok, &pkt->hdr, buf, buflen)))
        return rc;
	  coap_dumpToken(&pkt->tok);
    pkt->numopts = MAXOPT;
    if (0 != (rc = coap_parseOptionsAndPayload(pkt->opts, &(pkt->numopts), &(pkt->payload), &pkt->hdr, buf, buflen)))
        return rc;
    coap_dumpOptions(pkt->opts,pkt->numopts);
		coap_dumpPayload(&pkt->payload);
    return 0;
}

// options are always stored consecutively, so can return a block with same option num
const coap_option_t *coap_findOptions(const coap_packet_t *pkt, uint8_t num, uint8_t *count)
{
    // FIXME, options is always sorted, can find faster than this
    size_t i;
    const coap_option_t *first = NULL;
    *count = 0;
    for (i=0;i<pkt->numopts;i++)
    {
        if (pkt->opts[i].num == num)
        {
            if (NULL == first)
                first = &pkt->opts[i];
            (*count)++;
        }
        else
        {
            if (NULL != first)
                break;
        }
    }
    return first;
}

int coap_buffer_to_string(char *strbuf, size_t strbuflen, const coap_buffer_t *buf)
{
    if (buf->len+1 > strbuflen)
        return COAP_ERR_BUFFER_TOO_SMALL;
    memcpy(strbuf, buf->p, buf->len);
    strbuf[buf->len] = 0;
    return 0;
}

int coap_build(uint8_t *buf, size_t *buflen, const coap_packet_t *pkt)
{
    size_t opts_len = 0;
    size_t i;
    uint8_t *p;
    uint16_t running_delta = 0;

    // build header
    if (*buflen < (4U + pkt->hdr.tkl))
        return COAP_ERR_BUFFER_TOO_SMALL;

    buf[0] = (pkt->hdr.ver & 0x03) << 6;
    buf[0] |= (pkt->hdr.t & 0x03) << 4;
    buf[0] |= (pkt->hdr.tkl & 0x0F);
    buf[1] = pkt->hdr.code;
    buf[2] = pkt->hdr.id[0];
    buf[3] = pkt->hdr.id[1];

    // inject token
    p = buf + 4;
    if ((pkt->hdr.tkl > 0) && (pkt->hdr.tkl != pkt->tok.len))
        return COAP_ERR_UNSUPPORTED;
    
    if (pkt->hdr.tkl > 0)
        memcpy(p, pkt->tok.p, pkt->hdr.tkl);

    // // http://tools.ietf.org/html/rfc7252#section-3.1
    // inject options
    p += pkt->hdr.tkl;

    for (i=0;i<pkt->numopts;i++)     
    {
        uint32_t optDelta;
        uint8_t len, delta = 0;

        if (((size_t)(p-buf)) > *buflen)
             return COAP_ERR_BUFFER_TOO_SMALL;
        optDelta = pkt->opts[i].num - running_delta;    //running_delta 初始值为0 
        coap_option_nibble(optDelta, &delta);
        coap_option_nibble((uint32_t)pkt->opts[i].buf.len, &len);

        *p++ = (0xFF & (delta << 4 | len));
        if (delta == 13)
        {
            *p++ = (optDelta - 13);
        }
        else
        if (delta == 14)
        {
            *p++ = ((optDelta-269) >> 8);
            *p++ = (0xFF & (optDelta-269));
        }
        if (len == 13)
        {
            *p++ = (pkt->opts[i].buf.len - 13);
        }
        else
        if (len == 14)
  	    {
            *p++ = (pkt->opts[i].buf.len >> 8);
            *p++ = (0xFF & (pkt->opts[i].buf.len-269));
        }

        memcpy(p, pkt->opts[i].buf.p, pkt->opts[i].buf.len);
        p += pkt->opts[i].buf.len;
        running_delta = pkt->opts[i].num;
    }

    opts_len = (p - buf) - 4;   // number of bytes used by options

    if (pkt->payload.len > 0)
    {
        if (*buflen < 4 + 1 + pkt->payload.len + opts_len)
            return COAP_ERR_BUFFER_TOO_SMALL;
        buf[4 + opts_len] = 0xFF;  // payload marker
        memcpy(buf+5 + opts_len, pkt->payload.p, pkt->payload.len);
        *buflen = opts_len + 5 + pkt->payload.len;
    }
    else
        *buflen = opts_len + 4;
    return 0;
}

void coap_option_nibble(uint32_t value, uint8_t *nibble)
{
    if (value<13)
    {
        *nibble = (0xFF & value);
    }
    else
    if (value<=0xFF+13)
    {
        *nibble = 13;
    } else if (value<=0xFFFF+269)
    {
        *nibble = 14;
    }
}

int coap_make_response(coap_rw_buffer_t *scratch, coap_packet_t *pkt, const uint8_t *content, size_t content_len, uint8_t msgid_hi,
                      	uint8_t msgid_lo, const coap_buffer_t* tok, coap_responsecode_t rspcode, coap_content_type_t content_type)
{
    pkt->hdr.ver = 0x01;
    pkt->hdr.t = COAP_TYPE_ACK;
    pkt->hdr.tkl = 0;
    pkt->hdr.code = rspcode;
    pkt->hdr.id[0] = msgid_hi;
    pkt->hdr.id[1] = msgid_lo;
    pkt->numopts = 1;

    // need token in response
    if (tok) {
        pkt->hdr.tkl = tok->len;
        pkt->tok = *tok;
    }

    // safe because 1 < MAXOPT
    pkt->opts[0].num = COAP_OPTION_CONTENT_FORMAT;
    pkt->opts[0].buf.p = scratch->p;
    if (scratch->len < 2)
        return COAP_ERR_BUFFER_TOO_SMALL;
    scratch->p[0] = ((uint16_t)content_type & 0xFF00) >> 8;
    scratch->p[1] = ((uint16_t)content_type & 0x00FF);
    pkt->opts[0].buf.len = 2;
    pkt->payload.p = content;//
    pkt->payload.len = content_len;
    return 0;
}

// FIXME, if this looked in the table at the path before the method then
// it could more easily return 405 errors
int coap_handle_req(coap_rw_buffer_t *scratch, const coap_packet_t *inpkt, coap_packet_t *outpkt)
{
    const coap_option_t *opt;
    uint8_t count;
    int i;
    const coap_endpoint_t *ep = endpoints;

    while(NULL != ep->handler)
    {
        if (ep->method != inpkt->hdr.code)
            goto next;
        if (NULL != (opt = coap_findOptions(inpkt, COAP_OPTION_URI_PATH, &count)))
        {
            if (count != ep->path->count)
                goto next;
            for (i=0;i<count;i++)
            {
                if (opt[i].buf.len != strlen(ep->path->elems[i]))
                    goto next;
                if (0 != memcmp(ep->path->elems[i], opt[i].buf.p, opt[i].buf.len))
                    goto next;
            }
            // match!
						printf("找到了找到了\r\n");
            return ep->handler(scratch, inpkt, outpkt, inpkt->hdr.id[0], inpkt->hdr.id[1]);
        }
     next:
        ep++;
    }
    printf("没找到5555\r\n");
    coap_make_response(scratch, outpkt, NULL, 0, inpkt->hdr.id[0], inpkt->hdr.id[1], &inpkt->tok, COAP_RSPCODE_NOT_FOUND, COAP_CONTENTTYPE_NONE);

    return 0;
}

void coap_setup(void)
{
}

