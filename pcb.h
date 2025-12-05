#ifndef PCB_H
#define PCB_H

#include "types.h"
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
    uint8_t pstack[1024];
} pcb;

extern pcb proclist[MAXPROCS];

extern pcb *currproc;

extern pcb *idleproc;

#endif
