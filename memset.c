#ifndef MEMSET_C
#define MEMSET_C

#include "memset.h"

void *memset(void *ptr, char c, size_t n) {
    uint8_t *p = (uint8_t *)ptr;
    while (n--) {
        *p++ = c;
    }
    return ptr;
}

#endif
