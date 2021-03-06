#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "parser.h"

static int parse_uleb128(const unsigned char *s, uint32_t *ret){
    int i = 0;
    for( ; ; i++){
        char byte = s[i];
        *ret |= (byte & 0x7f) << (i * 7);
        if((byte & 0x80) == 0) break;
        
        // Prevent overflow
        if(i == 3) return -1;
    }
    return ++i;
}

osrstats *parse_osr(const unsigned char *s, int len){
    osrstats *ret = malloc(sizeof(osrstats));

    unsigned char *p = (unsigned char *)s;
    const void *end = s+len;

#define INC(x) do { \
    p+=x; \
    if((const void*)p >= end) \
        goto fail; \
    } while(0);


    // mode
    ret->mode = *p;
    INC(1);

    // don't care
    INC(4);

    // osu file MD5
    if(*p != 0x0B) goto fail;
    INC(1);
    if(*p != 0x20) goto fail;
    INC(1);
    memcpy(&ret->md5_map, p, sizeof(MD5));
    INC(0x20);

    // user name
    {
        if(*p != 0x0B) goto fail;
        INC(1);
        // FIXME: username len > 128
        uint8_t len = *p;
        if(len >= 128) goto fail;
        INC(1);
        memcpy(&ret->username, p, len);
        ret->username[len] = 0;
        INC(len);
    }

    // replay MD5
    if(*p != 0x0B) goto fail;
    INC(1);
    if(*p != 0x20) goto fail;
    INC(1);
    memcpy(&ret->md5_osr, p, sizeof(MD5));
    INC(0x20);

    // Stats
    ret->n_300  = *(uint16_t *)p; INC(2);
    ret->n_100  = *(uint16_t *)p; INC(2);
    ret->n_50   = *(uint16_t *)p; INC(2);
    ret->n_geki = *(uint16_t *)p; INC(2);
    ret->n_katu = *(uint16_t *)p; INC(2); 
    ret->n_miss = *(uint16_t *)p; INC(2); 

    ret->score = *(uint32_t *)p; INC(4); 
    ret->max_combo = *(uint16_t *)p; INC(2); 
    ret->perfect = *(bool *)p; INC(1);
    ret->mods = *(uint32_t *)p; INC(4); 

    // HP Graph
    {
        if(*p != 0x0B) goto fail;
        INC(1);
        uint32_t len;
        int n = parse_uleb128(p, &len);
        INC(n);
        // TODO
        INC(len);
    }
    
    
    // Datetime
    uint64_t rawtime = *(uint64_t *)p - 621355968000000000;
    ret->achieve_time.tv_sec = rawtime / 10000000;
    ret->achieve_time.tv_usec = rawtime % 10000000;
    INC(8)

    return ret;

#undef INC

fail:
    free(ret);
    return NULL;
}
