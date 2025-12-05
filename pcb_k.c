#include "pcb_k.h"

pcb proclist[MAXPROCS];//pcb memory voor de stackjes
pcb *currproc = NULL;
pcb *idleproc = NULL;

pcb *spawn_proc(uint32_t entrypoint) {
    pcb *p = NULL;
    
    // Zoek vrije plek
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
    
    // Align op 4 bytes in C
    if (((uintptr_t)sp & 0x3) != 0) {
        sp = (uint32_t*)(((uintptr_t)sp) & ~(uint32_t)0x3);
    }
    
    // Push 12 nullen voor s11-s0 want deze zijn nu nog leeg
    for (int i = 0; i < 12; i++) {
        sp--;
        *sp = 0;
    }
    
    // Push entrypoint als ra 
    sp--;
    *sp = (uint32_t)entrypoint;
    
    p->psp = (uint32_t)(uintptr_t)sp;
    
    return p;
}
