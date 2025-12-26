#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"
#include "mem.h"
#include "panic_k.h"
#include "pte.h"
#include "pte_utils.h"
#include "trap_frame_k.h"

#define PAGEFRAMESIZE 4096U

#define IS_PAGE_ALIGNED(addr) (((addr) & (PAGEFRAMESIZE - 1)) == 0)

// Kernel virtual memory window configuration
#define KERNEL_VIRT_BASE    0xC0000000  // Where kernel window starts in virtual memory
#define KERNEL_PHYS_BASE    0x80000000  // Where physical RAM actually starts
#define KERNEL_OFFSET       (KERNEL_VIRT_BASE - KERNEL_PHYS_BASE)  // 0x40000000

// Physical RAM size to map (128MB should be plenty for most embedded systems)
#define KERNEL_RAM_SIZE     0x08000000  // 128MB

// Conversion macros
#define PHYS_TO_VIRT(paddr) ((paddr) + KERNEL_OFFSET)
#define VIRT_TO_PHYS(vaddr) ((vaddr) - KERNEL_OFFSET)

// Check if address is in kernel window
#define IS_KERNEL_VIRT(addr) ((addr) >= KERNEL_VIRT_BASE)

typedef struct free_page_node {
    struct free_page_node *next;
} free_page_node_t;

// Function declarations
void init_memory(void);
void set_mmu_active(void);
uint32_t is_mmu_active(void);  // ADD THIS LINE
pframe_addr_t pageframalloc(uint32_t numpages);
void pageframfree(pframe_addr_t pfaddr, uint32_t numpages);
void add_ptbl_entry(uint32_t *pdbr, uint32_t vpa, uint32_t pfa, uint32_t pteflags);
void free_proc_pages(uint32_t *pdbr);
void map_kernel_window(uint32_t *pdbr);

#endif
