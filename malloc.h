#ifndef MALLOC
#define MALLOC

#include "types.h"
#include "panic_k.h"
#include "mem.h"
#include "memory.h"

extern uint8_t *mallocbuffer;

uint8_t *malloc(uint32_t size);

void init_malloc(void);

void free(uint8_t *p);

#endif
