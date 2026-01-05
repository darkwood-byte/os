#ifndef PCB_H
#define PCB_H

#include "types.h"
#include "deps.h"
#include "trap_frame_k.h"
#include "memory.h"
#include "csr.h"

#define MAXPROCS 6

typedef enum {
    NOPROC,
    READY,
    RUNNING,
    BLOCKED
} procstate;

typedef struct {
    uint32_t pid;
    uint32_t parent_id;
    procstate pstate;
    uint32_t psp;
    uint32_t *pdbr;
    uint8_t pstack[1024];
} pcb;

extern pcb proclist[MAXPROCS];

extern pcb *currproc;

extern pcb *idleproc;

pcb *spawn_proc(uint32_t  image, uint32_t imagesize);

void switch_umode(void);

void free_proc(pcb *p);

#endif
