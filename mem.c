#ifndef MEM_C
#define MEM_C

#include "mem.h"

void *memset(void *ptr, char c, size_t n) {
    uint8_t *p = (uint8_t *)ptr;
    while (n--) {
        *p++ = c;
    }
    return ptr;
}

void k_memcpy(void *dest, const void *src, size_t n){
    uint8_t *end = ( uint8_t *)dest;
    uint8_t *start = ( uint8_t *)src;
   
    for( uint32_t index = 0; index <( uint32_t) n; index++){
        *end = *start;
        end++;
        start++;
    }
}
#endif
