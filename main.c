#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "parser.h"

int main(int argc, char *argv[]){
    if(argc != 2){
        printf(
            "osrc -- osr parser\n"
            "\n"
            "%s [osr file]\n",
            argv[0]
        );
        return -1;
    }

    FILE *fp = fopen(argv[1],"rb");
    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    rewind(fp);
    unsigned char *s = malloc(len);
    fread(s, len, 1, fp);


    osrstats *stats = parse_osr(s, len);
    if(stats == NULL){
        puts("Parse file failed");
        return -1;
    }

    printf("Filename: %s\n", argv[1]);
    printf("========\n");
    printf("Player: %s\n", stats->username);
    printf("Time: %s\n", asctime(localtime(&stats->achieve_time.tv_sec)));
    printf("Mode: %d\n", stats->mode);
    printf("MD5 of map: %.32s\n", stats->md5_map);
    printf("MD5 of replay: %.32s\n", stats->md5_osr);
    printf("Mods: %u\n", stats->mods);
    printf("score: %u\n", stats->score);
    printf("combo: %u\n", stats->max_combo);
    printf("300: %u\n", stats->n_300);
    printf("100: %u\n", stats->n_100);
    printf("50: %u\n", stats->n_50);
    printf("geki: %u\n", stats->n_geki);
    printf("katu: %u\n", stats->n_katu);
    printf("miss: %u\n", stats->n_miss);


    free(stats);
    free(s);
    return 0;
}
