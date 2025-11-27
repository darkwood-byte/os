#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"
#include "mem.h"
#include "panic_k.h"
#include "trap_frame_k.h"

void init_memory(void);

pframe_addr_t pageframalloc(uint32_t numpages);

#endif // MEMORY_H
