#include "pcb_k.h"

extern char __kernel_base[];
extern char __free_ram_start[], __free_ram_end[];

pcb proclist[MAXPROCS];
pcb *currproc = NULL;
pcb *idleproc = NULL;

extern uint32_t kernel_main;

#define USR_BASE_VA 0x08000000
#define SSTAT_SPIE (1 << 5)

__attribute__((noreturn))
void switch_umode(void) {
    __asm__ __volatile__(
        "csrw sepc, %0\n"
        "csrw sstatus, %1\n"
        "sret\n"
        :
        : "r" (USR_BASE_VA), "r" (SSTAT_SPIE)
    );
    
    k_panic("u_mode no return return.\n", "");
}

pcb *spawn_proc(uint32_t image, uint32_t imagesize) {
    pcb *p = NULL;
    
    for(uint32_t i = 0; i < MAXPROCS; i++) {
        if(proclist[i].pstate == NOPROC) {
            p = &proclist[i];
            p->pid = i;
            break;
        }
    }
    
    if(!p) {
        k_panic("No free PCB, max process count: %d\n", MAXPROCS);
        return NULL;
    }
    
    memset(p->pstack, 0, sizeof(p->pstack));
    p->pstate = READY;
    
    uintptr_t stack_top = (uintptr_t)&p->pstack[0] + sizeof(p->pstack);
    uint32_t *sp = (uint32_t*)stack_top;
    
    if (((uintptr_t)sp & 0x3) != 0) {
        sp = (uint32_t*)(((uintptr_t)sp) & ~(uint32_t)0x3);
    }
    
    for (int i = 0; i < 12; i++) {
        sp--;
        *sp = 0;
    }
    
    sp--;
    if (image == (uint32_t)NULL && imagesize == 0) {
        *sp = (uint32_t)kernel_main;
         p->parent_id = 0;
    } else {
        *sp = (uint32_t)switch_umode;
         p->parent_id = currproc->pid;
    }

    p->psp = (uint32_t)(uintptr_t)sp;
    
    p->pdbr = (uint32_t *)pageframalloc(1);
    if (!p->pdbr) {
        k_panic("Failed to allocate PDBR for process\n", "");
        return NULL;
    }
    
    uint32_t page_count = 0;
    for (uint32_t pfa = (uint32_t)__kernel_base; 
         pfa < (uint32_t)__free_ram_end; 
         pfa += PAGEFRAMESIZE) {
        add_ptbl_entry(p->pdbr, pfa, pfa, PTE_FLG_R | PTE_FLG_W | PTE_FLG_X);
        page_count++;
    }
    
    if (image != (uint32_t)NULL && imagesize > 0) {
        
        uint32_t pages_allocated = 0;
        for (uint32_t bytecount = 0; bytecount < imagesize; bytecount += PAGEFRAMESIZE) {
            uint32_t pageframe = pageframalloc(1);
            if (!pageframe) {
                k_panic("Failed to allocate pageframe for user process\n", "");
                return NULL;
            }
            
            uint32_t bytes_to_copy = PAGEFRAMESIZE;
            uint32_t remaining = imagesize - bytecount;
            if (remaining < PAGEFRAMESIZE) {
                bytes_to_copy = remaining;
            }
            
            k_memcpy((void *)pageframe, (void *)(image + bytecount), bytes_to_copy);
            
            add_ptbl_entry(p->pdbr, 
                          (USR_BASE_VA + bytecount), 
                          pageframe, 
                          PTE_FLG_U | PTE_FLG_R | PTE_FLG_W | PTE_FLG_X);
            pages_allocated++;
        }

    } 
    return p;
}

void free_proc(pcb *p) {
    if (p == NULL) return;
    if (p->pstate == NOPROC) return;
    
    if (p->pdbr != NULL) {
        free_proc_pages(p->pdbr);
        p->pdbr = NULL;
    }
    
    p->pstate = NOPROC;
    p->pid = 0;
    p->parent_id = 0;
    p->psp = 0;
    memset(p->pstack, 0, sizeof(p->pstack));
}
