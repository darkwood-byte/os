#include "pcb_k.h"

extern char __kernel_base[];
extern char __free_ram_start[];

pcb proclist[MAXPROCS];//pcb memory voor de stackjes
pcb *currproc = NULL;
pcb *idleproc = NULL;



pcb *spawn_proc(uint32_t entrypoint) {
    pcb *p = NULL;
    
    // Zoek een mooie vrije plek
    for(uint32_t i = 0; i < MAXPROCS; i++) {
        if(proclist[i].pstate == NOPROC) {
            p = &proclist[i];
            p->pid = i;
            break;
        }
    }
    
    if(!p) {
        k_panic("No free PCB, max pc count: %d ", MAXPROCS);
        return NULL;
    }
    
    memset(p->pstack, 0, sizeof(p->pstack));
    p->pstate = READY;
    
    // Bereken stack top voor stack pointer
    uintptr_t stack_top = (uintptr_t)&p->pstack[0] + sizeof(p->pstack);
    uint32_t *sp = (uint32_t*)stack_top;
    
    // Align op 4 bytes 
    if (((uintptr_t)sp & 0x3) != 0) {
        sp = (uint32_t*)(((uintptr_t)sp) & ~(uint32_t)0x3);
    }
    
    // Push 12 nullen 
    for (int i = 0; i < 12; i++) {
        sp--;
        *sp = 0;
    }
    
    // Pushde  entrypoint als ra 
    sp--;
    *sp = (uint32_t)entrypoint;
    
    p->psp = (uint32_t)(uintptr_t)sp;
    
    // Page Directory
    p->pdbr = (uint32_t *)pageframalloc(1);
    
    if (!p->pdbr) {
        k_panic("Failed to allocate PDBR for process. . .", "");
        return NULL;
    }
    
    // Map kernel memory (identity mapping: VPA = PFA)
    k_printf("Mapping kernel memory for PID %d:\n", p->pid);
    for (uint32_t vpa = (uint32_t)__kernel_base; 
         vpa < (uint32_t)__free_ram_start; 
         vpa += PAGEFRAMESIZE) {
        add_ptbl_entry(p->pdbr, vpa, vpa, PTE_FLG_R | PTE_FLG_W | PTE_FLG_X);
    }
    k_printf("Mapped 0x%x - 0x%x \n", 
             (uint32_t)__kernel_base, (uint32_t)__free_ram_start);
    
    return p;
}
