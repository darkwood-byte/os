#include "malloc.h"

uint8_t *mallocbuffer;
static uint8_t mallocmetabuffer[MALSEGS];

static uint8_t *firstfit(uint32_t segs){
    uint32_t t = segs;
    for(uint32_t i = 0; i < MALSEGS; i++){
        if(mallocmetabuffer[i] == 0){
            t--;
            if(t == 0){
                for(uint32_t j = 0; j < segs; j++)if (j == 0)mallocmetabuffer[i - j] = (uint8_t)segs + 1; else mallocmetabuffer[i - j] = 1;
                return  mallocbuffer + SEGSIZE * (i - segs + 1);;
            }
        }
        else t = segs;
    }
    return NULL;
}

uint8_t *malloc(uint32_t size){
    if (!size) return NULL;
    return firstfit((size + SEGSIZE - 1) / SEGSIZE);
}

void init_malloc(void){
    mallocbuffer = (uint8_t *)pageframalloc(MALOCPAGECOUNT);
    memset(mallocbuffer, 0, MALBUFFERSIZE);
    memset(mallocmetabuffer, 0, MALSEGS);
}

void free(uint8_t *p){
    if(!p)return;
    if (p < mallocbuffer || p >= mallocbuffer + MALBUFFERSIZE) return;
    if((p - mallocbuffer)% SEGSIZE)return;

    uint32_t current_seg = (uint32_t)(p - mallocbuffer) / SEGSIZE;
    uint32_t seg_ammount = mallocmetabuffer[current_seg] - 1;

    if(mallocmetabuffer[current_seg] <= 1)return;

    memset(p, 0, seg_ammount * SEGSIZE);
    memset(mallocmetabuffer + current_seg, 0, seg_ammount);
}
