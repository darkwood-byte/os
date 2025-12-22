#include "pcb_k.h"
#include "csr.h"

extern char __kernel_base[];
extern char __free_ram_start[], __free_ram_end[];

pcb proclist[MAXPROCS];//pcb memory voor de stackjes
pcb *currproc = NULL;
pcb *idleproc = NULL;

extern uint32_t kernel_main;

    #define USR_BASE_VA 0x08000000
    #define SSTAT_SPIE (1 << 5)  // SPIE-bit = 1

__attribute__((noreturn))
void switch_umode(void) {
    k_printf("entering usermode\n");
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
    
    k_printf("spawn_proc called with image=%p, size=%p\n", image, imagesize);
    
    for(uint32_t i = 0; i < MAXPROCS; i++) {//pcb
        if(proclist[i].pstate == NOPROC) {
            p = &proclist[i];
            p->pid = i;
            k_printf("Found free PCB slot at PID %d\n", i);
            break;
        }
    }
    
    if(!p) {
        k_panic("No free PCB, max process count: %d\n", MAXPROCS);
        return NULL;
    }
    
    // stack
    k_printf("Initializing stack for PID %d\n", p->pid);
    memset(p->pstack, 0, sizeof(p->pstack));
    p->pstate = READY;
    
    //regs
    uintptr_t stack_top = (uintptr_t)&p->pstack[0] + sizeof(p->pstack);
    uint32_t *sp = (uint32_t*)stack_top;
    
    // Align op 4 bytes 
    if (((uintptr_t)sp & 0x3) != 0) {
        sp = (uint32_t*)(((uintptr_t)sp) & ~(uint32_t)0x3);
    }
    
    // Push 12 nullen (S11 t/m S0)
    for (int i = 0; i < 12; i++) {
        sp--;
        *sp = 0;
    }
    
    //  entrypoint
    sp--;
    if (image == (uint32_t)NULL && imagesize == 0) {
        // PID 0: idle proces, geen user mode
        k_printf("PID %d: Setting kernel_main as entrypoint\n", p->pid);
        *sp = (uint32_t)kernel_main;
    } else {
        // User mode proces: gebruik switch_umode hier wel
        k_printf("PID %d: Setting switch_umode as entrypoint\n", p->pid);
        *sp = (uint32_t)switch_umode;
    }
    
    p->psp = (uint32_t)(uintptr_t)sp;
    k_printf("PID %d: PSP set to %p\n", p->pid, p->psp);
    
    // 4. Maak Page Directory aan en page het GEHELE kernel-proces
    k_printf("PID %d: Allocating page directory\n", p->pid);
    p->pdbr = (uint32_t *)pageframalloc(1);
    if (!p->pdbr) {
        k_panic("Failed to allocate PDBR for process\n", "");
        return NULL;
    }
    k_printf("PID %d: PDBR allocated at %p\n", p->pid, p->pdbr);
    
    // Map kernel memory (identity mapping: VPA = PFA)
    k_printf("PID %d: Mapping kernel memory from %p to %p\n", 
             p->pid, (uint32_t)__kernel_base, (uint32_t)__free_ram_end);
    
    uint32_t page_count = 0;
    for (uint32_t pfa = (uint32_t)__kernel_base; 
         pfa < (uint32_t)__free_ram_end; 
         pfa += PAGEFRAMESIZE) {
        add_ptbl_entry(p->pdbr, pfa, pfa, PTE_FLG_R | PTE_FLG_W | PTE_FLG_X);
        page_count++;
    }
    k_printf("PID %d: Mapped %d kernel pages\n", p->pid, page_count);
    
    // 5. NIEUWE STAP: Alloceer, kopiëer EN page het GEHELE user-proces
    if (image != (uint32_t)NULL && imagesize > 0) {
        k_printf("PID %d: Loading user binary (size=%d bytes)\n", p->pid, imagesize);
        
        uint32_t pages_allocated = 0;
        for (uint32_t bytecount = 0; bytecount < imagesize; bytecount += PAGEFRAMESIZE) {
            uint32_t pageframe = pageframalloc(1);
            if (!pageframe) {
                k_panic("Failed to allocate pageframe for user process\n", "");
                return NULL;
            }
            
            // Bereken hoeveel bytes er nog gekopieerd moeten worden
            uint32_t bytes_to_copy = PAGEFRAMESIZE;
            uint32_t remaining = imagesize - bytecount;
            if (remaining < PAGEFRAMESIZE) {
                bytes_to_copy = remaining;
            }
            
            // Kopieer de bytes van de binary image naar de nieuwe pageframe
            k_memcpy((void *)pageframe, (void *)(image + bytecount), bytes_to_copy);
            
            // Map de nieuwe page naar USR_BASE_VA en verder
            add_ptbl_entry(p->pdbr, 
                          (USR_BASE_VA + bytecount), 
                          pageframe, 
                          PTE_FLG_U | PTE_FLG_R | PTE_FLG_W | PTE_FLG_X);
            pages_allocated++;
        }
        k_printf("PID %d: Allocated and mapped %d user pages starting at %p\n", 
                 p->pid, pages_allocated, USR_BASE_VA);
    } else {
        k_printf("PID %d: No user binary to load (idle process)\n", p->pid);
    }
    
    k_printf("PID %d: Process spawned successfully!\n", p->pid);
    return p;
}
