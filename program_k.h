#ifndef PROGRAMK_H
#define PROGRAMK_H

#include "deps.h"
#include "pcb_k.h"

void switch_proc(uint32_t *current_psp, uint32_t *next_psp) ;

void yield(void) ;

void k_sp(void);

#endif
