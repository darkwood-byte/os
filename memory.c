#include "memory.h"

extern char __free_ram_start[];
extern char __free_ram_end[];
extern char __kernel_base[];

static pframe_addr_t nextfreepageframe = 0;
static free_page_node_t *free_list_head = NULL;
static uint32_t mmu_is_active = 0;  // Track if MMU is on

void init_memory(void) {
    nextfreepageframe = (pframe_addr_t)__free_ram_start;
    free_list_head = NULL;
    mmu_is_active = 0;  // MMU starts off
}

void set_mmu_active(void) {
    mmu_is_active = 1;
    k_printf("[DEBUG] MMU marked as active\n");
}

uint32_t is_mmu_active(void) {
    return mmu_is_active;
}

pframe_addr_t pageframalloc(uint32_t numpages) {
    if (numpages == 0) return 0;
    
    // DEBUG: Log allocation attempt
    k_printf("[DEBUG] pageframalloc: Requesting %d pages, mmu_active=%d\n", 
             numpages, mmu_is_active);
    
    if (numpages == 1 && free_list_head != NULL) {
        pframe_addr_t pfaddr = (pframe_addr_t)free_list_head;
        free_list_head = free_list_head->next;
        
        // DEBUG: Log allocation from free list
        k_printf("[DEBUG] pageframalloc: Allocated 0x%x from free list\n", pfaddr);
        
        // Clear the page (from free list contains garbage)
        if (!mmu_is_active) {
            // MMU is off - safe to clear directly
            memset((void*)pfaddr, 0, PAGEFRAMESIZE);
        } else {
            // MMU is on - use kernel window
            uint32_t vaddr = PHYS_TO_VIRT(pfaddr);
            memset((void*)vaddr, 0, PAGEFRAMESIZE);
        }
        return pfaddr;
    }
    
    if (numpages > 1 && free_list_head != NULL) {
        free_page_node_t *prev = NULL;
        free_page_node_t *curr = free_list_head;
        
        while (curr != NULL) {
            pframe_addr_t start_addr = (pframe_addr_t)curr;
            uint32_t found_count = 0;
            free_page_node_t *check = curr;
            
            while (check != NULL && found_count < numpages) {
                pframe_addr_t expected_addr = start_addr + (found_count * PAGEFRAMESIZE);
                if ((pframe_addr_t)check != expected_addr) {
                    break;
                }
                found_count++;
                check = check->next;
            }
            
            if (found_count == numpages) {
                if (prev == NULL) {
                    free_list_head = check;
                } else {
                    prev->next = check;
                }
                
                // DEBUG: Log contiguous allocation
                k_printf("[DEBUG] pageframalloc: Allocated contiguous 0x%x-%x from free list\n", 
                         start_addr, start_addr + (numpages * PAGEFRAMESIZE));
                
                // Clear the pages (from free list contains garbage)
                if (!mmu_is_active) {
                    memset((void*)start_addr, 0, numpages * PAGEFRAMESIZE);
                } else {
                    uint32_t vaddr = PHYS_TO_VIRT(start_addr);
                    memset((void*)vaddr, 0, numpages * PAGEFRAMESIZE);
                }
                return start_addr;
            }
            
            prev = curr;
            curr = curr->next;
        }
    }
    
    pframe_addr_t pfaddr = nextfreepageframe;
    uint32_t allocation_size = numpages * PAGEFRAMESIZE;
    
    // DEBUG: Log new allocation
    k_printf("[DEBUG] pageframalloc: Allocating new at 0x%x, size=%d bytes\n", 
             pfaddr, allocation_size);
    
    nextfreepageframe += allocation_size;
    
    if (nextfreepageframe > (pframe_addr_t)__free_ram_end) {
        k_panic("Out of memory! Geheugen vol koop een nuwe RAM, jonge!\n", "");
    }
    
    // For NEW allocations (not from free list), the memory should be zero
    // We only clear if MMU is off
    if (!mmu_is_active) {
        memset((void*)pfaddr, 0, allocation_size);
    }
    // If MMU is on, assume new memory is already zero (never been written)
    
    return pfaddr;
}

void pageframfree(pframe_addr_t pfaddr, uint32_t numpages) {
    if (pfaddr == 0 || numpages == 0) return;
    
    // DEBUG: Log free
    k_printf("[DEBUG] pageframfree: Freeing 0x%x, %d pages, mmu_active=%d\n", 
             pfaddr, numpages, mmu_is_active);
    
    if (!IS_PAGE_ALIGNED(pfaddr)) {
        k_panic("pageframfree: address not page aligned: %p\n", pfaddr);
    }
    
    for (uint32_t i = 0; i < numpages; i++) {
        pframe_addr_t page_addr = pfaddr + (i * PAGEFRAMESIZE);
        
        // Add to free list
        if (mmu_is_active) {
            // Access through kernel window
            uint32_t vaddr = PHYS_TO_VIRT(page_addr);
            free_page_node_t *node = (free_page_node_t *)vaddr;
            node->next = free_list_head;
            free_list_head = node;
        } else {
            // Direct access
            free_page_node_t *node = (free_page_node_t *)page_addr;
            node->next = free_list_head;
            free_list_head = node;
        }
    }
}

void map_kernel_window(uint32_t *pdbr) {
    // We need to map the entire free RAM area so pageframalloc works
    // But mapping page-by-page causes infinite recursion
    // Solution: Map in larger chunks
    
    uint32_t kernel_start = (uint32_t)__kernel_base;
    uint32_t kernel_end = (uint32_t)__free_ram_start;
    uint32_t free_start = (uint32_t)__free_ram_start;
    uint32_t free_end = (uint32_t)__free_ram_end;
    
    k_printf("[DEBUG] map_kernel_window: pdbr=%p\n", pdbr);
    k_printf("Creating kernel window:\n");
    k_printf("  Kernel area: 0x%x - 0x%x (%d KB)\n", 
             kernel_start, kernel_end, (kernel_end - kernel_start) >> 10);
    k_printf("  Free RAM: 0x%x - 0x%x (%d MB)\n", 
             free_start, free_end, (free_end - free_start) >> 20);
    
    // Map kernel area page-by-page (small area)
    for (uint32_t paddr = kernel_start; paddr < kernel_end; paddr += PAGEFRAMESIZE) {
        // Identity mapping for kernel code
        add_ptbl_entry(pdbr, paddr, paddr, PTE_FLG_R | PTE_FLG_W | PTE_FLG_X);
        
        // Kernel window mapping
        uint32_t vaddr = PHYS_TO_VIRT(paddr);
        add_ptbl_entry(pdbr, vaddr, paddr, PTE_FLG_R | PTE_FLG_W | PTE_FLG_X);
    }
    
    // Map free RAM in 1MB chunks to reduce page table allocations
    // Start from 1MB-aligned address
    uint32_t chunk_start = free_start & ~0xFFFFF;  // Align down to 1MB
    uint32_t chunk_end = (free_end + 0xFFFFF) & ~0xFFFFF;  // Align up to 1MB
    
    for (uint32_t chunk = chunk_start; chunk < chunk_end; chunk += 0x100000) {
        // For each 1MB chunk, map it as kernel window only
        uint32_t vaddr = PHYS_TO_VIRT(chunk);
        
        // Map each 4KB page in the chunk
        for (uint32_t offset = 0; offset < 0x100000; offset += PAGEFRAMESIZE) {
            uint32_t curr_paddr = chunk + offset;
            uint32_t curr_vaddr = vaddr + offset;
            
            // Skip if outside actual free RAM
            if (curr_paddr < free_start || curr_paddr >= free_end) {
                continue;
            }
            
            // Skip kernel area (already mapped)
            if (curr_paddr >= kernel_start && curr_paddr < kernel_end) {
                continue;
            }
            
            // Map only kernel window (no identity mapping for free RAM)
            add_ptbl_entry(pdbr, curr_vaddr, curr_paddr, PTE_FLG_R | PTE_FLG_W | PTE_FLG_X);
        }
    }
    
    k_printf("Kernel window mapping complete!\n");
}

void add_ptbl_entry(uint32_t *pdbr, uint32_t vpa, uint32_t pfa, uint32_t pteflags) {
    if (!IS_PAGE_ALIGNED(vpa) || !IS_PAGE_ALIGNED(pfa)) {
        k_printf("ERROR: VPA of PFA not page aligned!\n");
        k_printf("VPA: %x", vpa);
        k_printf(", PFA: %x\n", pfa);
        k_panic("Page alignment check failed", "");
    }
    
    // pdbr is a pointer to the page directory
    uint32_t *pdbr_access = pdbr;
    
    // If MMU is NOT active and pdbr is a kernel virtual address, convert it
    if (!mmu_is_active && ((uint32_t)pdbr >= KERNEL_VIRT_BASE)) {
        // pdbr is a kernel virtual address, convert to physical
        uint32_t pdbr_phys = VIRT_TO_PHYS((uint32_t)pdbr);
        pdbr_access = (uint32_t *)pdbr_phys;
    }
    
    uint32_t vpn1 = VPN1_FROM_VPA(vpa);
    uint32_t *pde_ptr = &pdbr_access[vpn1];
    uint32_t pde_value = *pde_ptr;
    
    if (!PDE_IS_VALID(pde_value)) {
        // Allocate sub-table (returns physical address)
        uint32_t sub_table_phys = pageframalloc(1);
        if (!sub_table_phys) {
            k_panic("Failed to allocate sub-page-table", "");
        }
        
        if (!IS_PAGE_ALIGNED(sub_table_phys)) {
            k_panic("pageframalloc returned non-aligned address", "");
        }
        
        uint32_t sub_table_ppn = PPN_FROM_PFA(sub_table_phys);
        uint32_t new_pde = (sub_table_ppn << 10) | PTE_FLG_V;
        
        *pde_ptr = new_pde;
        pde_value = new_pde;
    }
    
    // Extract PPN from PDE (bits [31:10])
    uint32_t sub_table_ppn = (pde_value >> 10) & 0x003FFFFF;
    uint32_t sub_table_phys = sub_table_ppn << 12;
    
    if (!IS_PAGE_ALIGNED(sub_table_phys)) {
        k_panic("sPTBR not aligned from PDE: 0x%x", sub_table_phys);
    }
    
    // Access sub-table
    uint32_t *sub_table;
    if (mmu_is_active) {
        uint32_t sub_table_virt = PHYS_TO_VIRT(sub_table_phys);
        sub_table = (uint32_t *)sub_table_virt;
    } else {
        sub_table = (uint32_t *)sub_table_phys;
    }
    
    uint32_t vpn0 = VPN0_FROM_VPA(vpa);
    uint32_t *pte_ptr = &sub_table[vpn0];
    uint32_t target_ppn = PPN_FROM_PFA(pfa);
    uint32_t pte = (target_ppn << 10) | pteflags | PTE_FLG_V;
    
    *pte_ptr = pte;
}

void free_proc_pages(uint32_t *pdbr) {
    if (pdbr == NULL) {
        k_printf("[DEBUG] free_proc_pages: NULL pdbr\n");
        return;
    }
    
    uint32_t kernel_start = (uint32_t)__kernel_base;
    uint32_t kernel_end = (uint32_t)__free_ram_start;
    
    for (uint32_t vpn1 = 0; vpn1 < 1024; vpn1++) {
        uint32_t pde = pdbr[vpn1];
        
        if (!PDE_IS_VALID(pde)) continue;
        
        uint32_t sub_table_ppn = (pde >> 10) & 0x003FFFFF;
        uint32_t sub_table_pfa = sub_table_ppn << 12;
        
        // Access sub-table
        uint32_t *sub_table;
        if (mmu_is_active) {
            uint32_t sub_table_virt = PHYS_TO_VIRT(sub_table_pfa);
            sub_table = (uint32_t *)sub_table_virt;
        } else {
            sub_table = (uint32_t *)sub_table_pfa;
        }
        
        for (uint32_t vpn0 = 0; vpn0 < 1024; vpn0++) {
            uint32_t pte = sub_table[vpn0];
            
            if (!(pte & PTE_FLG_V)) continue;
            if (!(pte & PTE_FLG_U)) continue;
            
            uint32_t page_ppn = (pte >> 10) & 0x003FFFFF;
            uint32_t page_pfa = page_ppn << 12;
            
            if (page_pfa >= kernel_start && page_pfa < kernel_end) {
                continue;
            }
            
            pageframfree(page_pfa, 1);
        }
        
        if (sub_table_pfa >= kernel_start && sub_table_pfa < kernel_end) {
            continue;
        }
        
        pageframfree(sub_table_pfa, 1);
    }
    
    uint32_t pdbr_pfa = VIRT_TO_PHYS((uint32_t)pdbr);
    if (pdbr_pfa >= kernel_start && pdbr_pfa < kernel_end) {
        return;
    }
    
    pageframfree(pdbr_pfa, 1);
}
