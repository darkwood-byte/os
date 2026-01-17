#include "memory.h"

extern char __free_ram_start[];
extern char __free_ram_end[];
extern char __kernel_base[];

static pframe_addr_t nextfreepageframe = 0;

void init_memory(void) {
    nextfreepageframe = (pframe_addr_t)__free_ram_start;
}

pframe_addr_t pageframalloc(uint32_t numpages) {
    // Save the current address as the returnvalue
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
    
    // Return address of first page frame
    return pfaddr;
}

void add_ptbl_entry(uint32_t *pdbr, uint32_t vpa, uint32_t pfa, uint32_t pteflags) {
    if (!IS_PAGE_ALIGNED(vpa) || !IS_PAGE_ALIGNED(pfa)) {
        k_printf("ERROR: VPA of PFA not page aligned!\n");
        k_printf("VPA: %x", vpa);
        k_printf(", PFA: %x", pfa);
        k_printf("\n");
        k_panic("Page alignment check failed", "");
    }
    
    uint32_t vpn1 = VPN1_FROM_VPA(vpa);
    uint32_t *pde_ptr = &pdbr[vpn1];
    uint32_t pde_value = *pde_ptr;
    
    if (!PDE_IS_VALID(pde_value)) {
        uint32_t *sub_table = (uint32_t *)pageframalloc(1);
        if (!sub_table) {
            k_panic("Failed to allocate sub-page-table", "");
        }
        
        if (!IS_PAGE_ALIGNED((uint32_t)sub_table)) {
            k_panic("pageframalloc returned non-aligned address", "");
        }
        
        uint32_t sub_table_ppn = PPN_FROM_PFA((uint32_t)sub_table);
        uint32_t new_pde = (sub_table_ppn << 10) | PTE_FLG_V;
        *pde_ptr = new_pde;
        pde_value = new_pde;
    }
    
    uint32_t sub_table_ppn = PPN_FROM_ENTRY(pde_value);
    uint32_t sptbr = PFA_FROM_PPN(sub_table_ppn);
    
    if (!IS_PAGE_ALIGNED(sptbr)) {
        k_panic("sPTBR not aligned from PDE", "");
    }
    
    uint32_t vpn0 = VPN0_FROM_VPA(vpa);
    uint32_t *sub_table = (uint32_t *)sptbr;
    uint32_t *pte_ptr = &sub_table[vpn0];
    
    uint32_t target_ppn = PPN_FROM_PFA(pfa);
    uint32_t pte = (target_ppn << 10) | pteflags | PTE_FLG_V;
    *pte_ptr = pte;
}

void free_proc_pages(uint32_t *pdbr) {
    if (pdbr == NULL) {
        return;
    }
    
    uint32_t kernel_start = (uint32_t)__kernel_base;
    uint32_t kernel_end = (uint32_t)__free_ram_start;
    
    for (uint32_t vpn1 = 0; vpn1 < 1024; vpn1++) {
        uint32_t pde = pdbr[vpn1];
        if (!PDE_IS_VALID(pde)) continue;
        
        uint32_t sub_table_ppn = PPN_FROM_ENTRY(pde);
        uint32_t sub_table_pfa = PFA_FROM_PPN(sub_table_ppn);
        uint32_t *sub_table = (uint32_t *)sub_table_pfa;
        
        for (uint32_t vpn0 = 0; vpn0 < 1024; vpn0++) {
            uint32_t pte = sub_table[vpn0];
            if (!(pte & PTE_FLG_V)) continue;
            if (!(pte & PTE_FLG_U)) continue;
            
            uint32_t page_ppn = PPN_FROM_ENTRY(pte);
            uint32_t page_pfa = PFA_FROM_PPN(page_ppn);
            
            if (page_pfa >= kernel_start && page_pfa < kernel_end) continue;
            
            // markeer invalid
            sub_table[vpn0] = 0;
        }
        
        if (sub_table_pfa >= kernel_start && sub_table_pfa < kernel_end) continue;
        
        // Markeer invalid
        pdbr[vpn1] = 0;
    }
    
    // PDBR zelf wordt niet vrijgegeven bij kernel!!!!
    uint32_t pdbr_pfa = (uint32_t)pdbr;
    if (pdbr_pfa >= kernel_start && pdbr_pfa < kernel_end) {
        return;
    }
    
}
