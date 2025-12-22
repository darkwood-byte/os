#ifndef PCB_H
#define PCB_H

#include "types.h"
#include "deps.h"
#include "trap_frame_k.h"//voor pageframe size
#include "memory.h"

#define MAXPROCS 4

typedef enum {
    NOPROC,
    READY,
    RUNNING,
    BLOCKED
} procstate;

typedef struct {
    uint32_t pid;
    procstate pstate;
    uint32_t psp;
    uint32_t *pdbr; // <=== Process 'page directory base register'
    uint8_t pstack[1024];
} pcb;

extern pcb proclist[MAXPROCS];

extern pcb *currproc;

extern pcb *idleproc;

pcb *spawn_proc(uint32_t  image, uint32_t imagesize);

void switch_umode(void);

#endif
