// memory.h
#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

void memory_init(void *start, size_t size);
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t size);
size_t get_free_memory(void);
size_t get_used_memory(void);

#endif
