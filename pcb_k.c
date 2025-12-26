#include "pcb_k.h"
#include "memory.h"
#include "print_k.h"
#include "mem.h"

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
    
    // Find free PCB slot
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
    
    // Initialize process stack
    memset(p->pstack, 0, sizeof(p->pstack));
    p->pstate = READY;
    
    // Set up kernel stack for context switching
    uintptr_t stack_top = (uintptr_t)&p->pstack[0] + sizeof(p->pstack);
    uint32_t *sp = (uint32_t*)stack_top;
    
    if (((uintptr_t)sp & 0x3) != 0) {
        sp = (uint32_t*)(((uintptr_t)sp) & ~(uint32_t)0x3);
    }
    
    // Push dummy saved registers (s0-s11, ra)
    for (int i = 0; i < 12; i++) {
        sp--;
        *sp = 0;
    }
    
    // Push return address
    sp--;
    if (image == (uint32_t)NULL && imagesize == 0) {
        *sp = (uint32_t)kernel_main;
        p->parent_id = 0;  // Kernel process
    } else {
        *sp = (uint32_t)switch_umode;
        p->parent_id = currproc ? currproc->pid : 0;  // User process
    }

    p->psp = (uint32_t)(uintptr_t)sp;
    
    // Allocate page directory (returns physical address)
    uint32_t pdbr_phys = pageframalloc(1);
    if (!pdbr_phys) {
        k_panic("Failed to allocate PDBR for process\n", "");
        return NULL;
    }
    
    // Store physical address
    p->pdbr_phys = pdbr_phys;
    
    // Convert to virtual address for kernel access
    if (is_mmu_active()) {
        p->pdbr_virt = (uint32_t *)PHYS_TO_VIRT(pdbr_phys);
    } else {
        p->pdbr_virt = (uint32_t *)pdbr_phys;
    }
    
    k_printf("Process %d: PDBR allocated at physical 0x%x (virtual %p)\n", 
             p->pid, p->pdbr_phys, p->pdbr_virt);
    
    // Create kernel window mapping
    map_kernel_window(p->pdbr_virt);
    
    // Map user program if provided
    if (image != (uint32_t)NULL && imagesize > 0) {
        k_printf("Loading user program for process %d (size: %d bytes)\n", p->pid, imagesize);
        
        uint32_t pages_allocated = 0;
        for (uint32_t bytecount = 0; bytecount < imagesize; bytecount += PAGEFRAMESIZE) {
            // Allocate physical page
            uint32_t pageframe_phys = pageframalloc(1);
            if (!pageframe_phys) {
                k_panic("Failed to allocate pageframe for user process\n", "");
                return NULL;
            }
            
            // Calculate how much to copy
            uint32_t bytes_to_copy = PAGEFRAMESIZE;
            uint32_t remaining = imagesize - bytecount;
            if (remaining < PAGEFRAMESIZE) {
                bytes_to_copy = remaining;
            }
            
            // Copy the program into the allocated page
            if (is_mmu_active()) {
                // MMU is ON - use kernel window virtual address
                uint32_t pageframe_virt = PHYS_TO_VIRT(pageframe_phys);
                k_memcpy((void *)pageframe_virt, (void *)(image + bytecount), bytes_to_copy);
            } else {
                // MMU is OFF - use physical address directly
                k_memcpy((void *)pageframe_phys, (void *)(image + bytecount), bytes_to_copy);
            }
            
            // Map this physical page into user virtual space
            add_ptbl_entry(p->pdbr_virt, 
                          (USR_BASE_VA + bytecount),  // User virtual address
                          pageframe_phys,             // Physical address
                          PTE_FLG_U | PTE_FLG_R | PTE_FLG_W | PTE_FLG_X);
            
            pages_allocated++;
        }
        
        k_printf("Process %d: Loaded %d pages into user space\n", p->pid, pages_allocated);
    } 
    
    k_printf("Process %d spawned successfully\n", p->pid);
    return p;
}

void free_proc(pcb *p) {
    if (p == NULL) return;
    if (p->pstate == NOPROC) return;
    
    k_printf("Freeing process %d (PDBR phys=0x%x, virt=%p)\n", 
             p->pid, p->pdbr_phys, p->pdbr_virt);
    
    if (p->pdbr_phys != 0) {
        // Free all pages owned by this process
        free_proc_pages(p->pdbr_virt);
        p->pdbr_phys = 0;
        p->pdbr_virt = NULL;
    }
    
    p->pstate = NOPROC;
    p->pid = 0;
    p->parent_id = 0;
    p->psp = 0;
    memset(p->pstack, 0, sizeof(p->pstack));
}
