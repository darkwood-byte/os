#ifndef PROGRAMK_H
#define PROGRAMK_H

#include "deps.h"
#include "pcb_k.h"

void switch_proc(uint32_t *current_psp, uint32_t *next_psp) ;

void yield(void) ;

void k_sp(void);

#define start(id)do{\
    proclist[id].pstate = start;\
    k_printf("started proces id: %d\n", id);\
}while(0);

#define block()do{\
    currproc->pstate = BLOCKED;\
    k_printf("blocked proces id:  %d\n", currproc->pid);\
    yield();\
}while(0);

#define kill()do{\
    currproc->pstate = NOPROC;\
    k_printf("killed proces id:  %d\n", currproc->pid);\
    yield();\
}while(0);

#endif
