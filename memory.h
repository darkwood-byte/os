#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"
#include "mem.h"
#include "panic_k.h"
#include "pte.h"
#include "trap_frame_k.h"

#define PAGEFRAMESIZE 4096U

#define IS_PAGE_ALIGNED(addr) (((addr) & (PAGEFRAMESIZE - 1)) == 0)

void init_memory(void);

pframe_addr_t pageframalloc(uint32_t numpages);

void add_ptbl_entry(uint32_t *pdbr, uint32_t vpa, uint32_t pfa, uint32_t pteflags);



#endif // MEMORY_H
