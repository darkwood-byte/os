#ifndef MEM_H
#define MEM_H

#include "types.h"

void *memset(void *ptr, char c, size_t n);

void k_memcpy(void *dest, const void *src, size_t n);

#endif
