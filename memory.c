#include "memory.h"

extern char __free_ram_start[];
extern char __free_ram_end[];
extern char __kernel_base[];

static pframe_addr_t nextfreepageframe = 0;
static free_page_node_t *free_list_head = NULL;

void init_memory(void) {
    nextfreepageframe = (pframe_addr_t)__free_ram_start;
    free_list_head = NULL;
}

pframe_addr_t pageframalloc(uint32_t numpages) {
    if (numpages == 0) return 0;//niks
    
    if (numpages == 1 && free_list_head != NULL) {
        pframe_addr_t pfaddr = (pframe_addr_t)free_list_head;
        free_list_head = free_list_head->next;
        memset((void*)pfaddr, 0, PAGEFRAMESIZE);
        return pfaddr;//enkele pagina
    }
    
    if (numpages > 1 && free_list_head != NULL) {//meedre paginaas
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
                
                memset((void*)start_addr, 0, numpages * PAGEFRAMESIZE);
                return start_addr;
            }
            
            prev = curr;
            curr = curr->next;
        }
    }
    
    pframe_addr_t pfaddr = nextfreepageframe;
    uint32_t allocation_size = numpages * PAGEFRAMESIZE;
    nextfreepageframe += allocation_size;
    
    if (nextfreepageframe > (pframe_addr_t)__free_ram_end) {
        k_panic("Out of memory! Geheugen vol koop een nuwe RAM, jonge!", "");
    }
    
    memset((void*)pfaddr, 0, allocation_size);
    return pfaddr;
}

void pageframfree(pframe_addr_t pfaddr, uint32_t numpages) {
    if (pfaddr == 0 || numpages == 0) return;//geen
    
    if (!IS_PAGE_ALIGNED(pfaddr)) {
        k_panic("pageframfree: address not page aligned: %p\n", pfaddr);
    }
    
    for (uint32_t i = 0; i < numpages; i++) {
        pframe_addr_t page_addr = pfaddr + (i * PAGEFRAMESIZE);
        
        free_page_node_t *node = (free_page_node_t *)page_addr;
        node->next = free_list_head;
        free_list_head = node;
    }
}

void add_ptbl_entry(uint32_t *pdbr, uint32_t vpa, uint32_t pfa, uint32_t pteflags) {
    if (!IS_PAGE_ALIGNED(vpa) || !IS_PAGE_ALIGNED(pfa)) {
        k_printf("ERROR: VPA of PFA not page aligned!\n");
        k_printf("VPA: %x", vpa);
        k_printf(", PFA: %x, pfa");
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
            
            pageframfree(page_pfa, 1);
        }
        
        if (sub_table_pfa >= kernel_start && sub_table_pfa < kernel_end) continue;
        
        pageframfree(sub_table_pfa, 1);
    }
    
    uint32_t pdbr_pfa = (uint32_t)pdbr;
    if (pdbr_pfa >= kernel_start && pdbr_pfa < kernel_end) {
        return;
    }
    
    pageframfree(pdbr_pfa, 1);
}
