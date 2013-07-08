#include <stdio.h>

typedef char MD5[32];
typedef enum {STD, TAIKO, CTB, MANIA} MODE;
typedef enum {
    NONFAIL    = 0x1,
    EAZY       = 0x2,
    NOVIDEO    = 0x4,
    HIDDEN     = 0x8,
    HARDROCK   = 0x10,
    ...
} MOD;


typedef struct {
    MODE mode;
    MD5 md5_map;
    char username[128];
    MD5 md5_osr;
    uint16_t n_300;
    uint16_t n_100;
    uint16_t n_50;
    uint16_t n_geki;
    uint16_t n_katu;
    uint16_t n_miss;
    uint32_t score;
    uint16_t max_combo;
    bool perfect;
    MOD mods;
    // TODO
    // HPGRAPH hpgraph;
    time_t time;
} oszstats;


int parse_uleb128(const char *s, uint32_t *ret){
    int i = 0;
    int shift = 0;
    for( ; ; i++){
        char byte = s[i];
        ret |= (byte & 0x7f) << (i * 7);
        if(byte & 0x80 == 0) break;
        
        // Prevent overflow
        if(i == 3) return -1;
    }
    return ++i;
}


oszstats *parse_oszstring(const char *s, int len){
    oszstats *ret = malloc(sizeof(oszstats));

    unsigned char *p = s;
    const void *end = s+len;

#define INC(x) do { \
    p+=x; \
    if(p >= end) \
        goto fail; \
    } while(0);


    // mode
    ret->mode = *(MODE *)p;
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
    if(*p != 0x0B) goto fail;
    INC(1);
    // FIXME: username len > 128
    uint8_t len = *p;
    if(len >= 128) goto fail;
    memcpy(&ret->username, p, len);
    ret->username[len] = 0;
    INC(len);

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
    if(*p != 0x0B) goto fail;
    INC(1);
    uint32_t len;
    int n = parse_uleb128(p, &len);
    INC(n);
    // TODO
    INC(len);
    
    
    // Datetime
    uint64_t *rawtime = *(uint64_t *)p;
    ret->time = *(uint64_t *)p / 10000000  - 62135596800;
    INC(8)

    return ret;

#undef INC

fail:
    free(ret);
    return NULL;
}
