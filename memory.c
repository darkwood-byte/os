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

void add_ptbl_entry(uint32_t *pdbr, uint32_t vpa, uint32_t pfa, uint32_t pteflags) {
    // Stap 1: Controleer page alignment
    if (!IS_PAGE_ALIGNED(vpa) || !IS_PAGE_ALIGNED(pfa)) {
        k_printf("ERROR: VPA of PFA not page aligned!\n");
        k_printf("VPA: %x", vpa);
        k_printf(", PFA: %x, pfa");
        k_printf("\n");
        k_panic("Page alignment check failed", "");
    }
    
    // Stap 2: Haal VPN[1] uit VPA en controleer of PDE geldig is
    uint32_t vpn1 = VPN1_FROM_VPA(vpa);
    
    // Adres van PDE in Page Directory
    uint32_t *pde_ptr = &pdbr[vpn1];  // PDBR is al een uint32_t pointer
    uint32_t pde_value = *pde_ptr;
    
    // Stap 3: Als PDE ongeldig is, alloceren we een nieuwe sub-page-table
    if (!PDE_IS_VALID(pde_value)) {
        // Alloceer een page frame voor de sub-page-table
        uint32_t *sub_table = (uint32_t *)pageframalloc(1);
        if (!sub_table) {
            k_panic("Failed to allocate sub-page-table", "");
        }
        
        // Controleer alignment van gealloceerde page
        if (!IS_PAGE_ALIGNED((uint32_t)sub_table)) {
            k_panic("pageframalloc returned non-aligned address", "");
        }
        
        // Maak de PDE: PPN van sub_table + Valid flag
        // sub_table is een PFA, haal er de PPN uit
        uint32_t sub_table_ppn = PPN_FROM_PFA((uint32_t)sub_table);
        
        // PDE bevat: PPN in bits 31:10, flags in bits 9:0
        // Voor een PDE (non-leaf PTE) moeten R,W,X bits 0 zijn volgens document
        uint32_t new_pde = (sub_table_ppn << 10) | PTE_FLG_V;
        
        // Schrijf de PDE naar de Page Directory
        *pde_ptr = new_pde;
        
        // Update pde_value voor verder gebruik
        pde_value = new_pde;
    }
    // Stap 4: Als PDE wel geldig is, doen we niets extra
    
    // Stap 5: Haal sPTBR en VPN[0] op
    // sPTBR = PFA van de sub-page-table (uit de PDE)
    uint32_t sub_table_ppn = PPN_FROM_ENTRY(pde_value);
    uint32_t sptbr = PFA_FROM_PPN(sub_table_ppn);  // Dit is de PFA van sub-page-table
    
    // Controleer alignment
    if (!IS_PAGE_ALIGNED(sptbr)) {
        k_panic("sPTBR not aligned from PDE", "");
    }
    
    // Haal VPN[0]
    uint32_t vpn0 = VPN0_FROM_VPA(vpa);
    
    // Bereken adres van PTE in sub-page-table
    uint32_t *sub_table = (uint32_t *)sptbr;
    uint32_t *pte_ptr = &sub_table[vpn0];
    
    // Stap 6: Schrijf de PTE
    // Haal PPN uit PFA parameter
    uint32_t target_ppn = PPN_FROM_PFA(pfa);
    
    // Maak PTE: PPN in bits 31:10, flags in bits 9:0
    // Gebruik meegegeven flags, maar zorg dat Valid bit ook aan staat
    uint32_t pte = (target_ppn << 10) | pteflags | PTE_FLG_V;
    
    // Schrijf PTE naar sub-page-table
    *pte_ptr = pte;
    
    // Optioneel: flush cache/TLB voor deze entry (later nodig bij context switches)
    // __asm__ __volatile__("sfence.vma zero, zero" : : : "memory");
}
