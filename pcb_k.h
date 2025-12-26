#ifndef PCB_K_H
#define PCB_K_H

#include "types.h"

#define MAXPROCS 10

typedef enum {
    NOPROC,
    RUNNING,
    READY,
    BLOCKED
} pstate_t;

// NO UNION - store separately
typedef struct pcb {
    uint32_t pid;
    uint32_t parent_id;
    pstate_t pstate;
    uint32_t psp;           // Process stack pointer (kernel stack)
    char pstack[8192];      // Kernel stack for this process
    uint32_t pdbr_phys;     // Physical address of page directory (for SATP)
    uint32_t *pdbr_virt;    // Virtual pointer to page directory (for kernel access)
} pcb;

extern pcb proclist[MAXPROCS];
extern pcb *currproc;
extern pcb *idleproc;

pcb *spawn_proc(uint32_t image, uint32_t imagesize);
void free_proc(pcb *p);

#endif
