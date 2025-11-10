#ifndef SLEEPK_H
#define SLEEPK_H

#define CPU_FREQ_HZ 2000000000  // 2000 MHz

#include "types.h"

static inline uint32_t read_cycles(void) ;

void sleepk_ms(uint32_t ms) ;

#endif
