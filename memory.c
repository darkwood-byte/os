#include "memory.h"


// External symbols from linker script
extern char __free_ram_start[];
extern char __free_ram_end[];

/**
 * Global pointer to the next available page frame
 * Initialized to __free_ram_start by init_memory()
 */
static pframe_addr_t nextfreepageframe = 0;

/**
 * init_memory - Initialize the memory management system
 * 
 * This must be called once during kernel boot before any
 * pageframalloc() calls are made.
 */
void init_memory(void) {
    nextfreepageframe = (pframe_addr_t)__free_ram_start;
}

/**
 * pageframalloc - Allocate physical page frames
 * 
 * Algorithm:
 * 1. Save current nextfreepageframe as return address
 * 2. Increment nextfreepageframe by (numpages * PAGEFRAMESIZE)
 * 3. Check if new value exceeds __free_ram_end (panic if so)
 * 4. Zero all allocated page frames
 * 5. Return saved address
 */
pframe_addr_t pageframalloc(uint32_t numpages) {
    // Step 4: Save current address as return value
    pframe_addr_t pfaddr = nextfreepageframe;
    
    // Step 5: Calculate size and increment pointer
    uint32_t allocation_size = numpages * PAGEFRAMESIZE;
    nextfreepageframe += allocation_size;
    
    // Step 6: Check for overflow
    if (nextfreepageframe > (pframe_addr_t)__free_ram_end) {
        k_panic("Out of memory! Geheugen vol koop een nuwe RAM, jonge!", "");
    }
    
    // Step 7: Zero all allocated page frames
    memset((void*)pfaddr, 0, allocation_size);
    
    // Step 8: Return address of first page frame
    return pfaddr;
}
