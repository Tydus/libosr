#ifndef __PARSER_H
#define __PARSER_H

#include <sys/time.h>
#include <stdint.h>
#include <stdbool.h>

typedef char MD5[32];
typedef enum {STD, TAIKO, CTB, MANIA} MODE;
typedef enum {
    NONFAIL    = 0x1,
    EAZY       = 0x2,
    NOVIDEO    = 0x4,
    HIDDEN     = 0x8,
    HARDROCK   = 0x10,
    //...
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
    struct timeval achieve_time;
} osrstats;

osrstats *parse_osr(const unsigned char *s, int len);

#endif
