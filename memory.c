#include "memory.h"


// linker script
extern char __free_ram_start[];
extern char __free_ram_end[];

static pframe_addr_t nextfreepageframe = 0;


void init_memory(void) {
    nextfreepageframe = (pframe_addr_t)__free_ram_start;
}

pframe_addr_t pageframalloc(uint32_t numpages) {
     //Save the current address as the returnvalue
    pframe_addr_t pfaddr = nextfreepageframe;
    
    // Calculate size and increment the main pointer
    uint32_t allocation_size = numpages * PAGEFRAMESIZE;
    nextfreepageframe += allocation_size;
    
    // Check for overflow
    if (nextfreepageframe > (pframe_addr_t)__free_ram_end) {
        k_panic("Out of memory! Geheugen vol koop een nuwe RAM, jonge!", "");
    }
    
    // Zero all allocated page frames
    memset((void*)pfaddr, 0, allocation_size);
    
    //  Return address of first page frame
    return pfaddr;
}
