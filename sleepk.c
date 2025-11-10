#ifndef SLEEPK_C
#define SLEEPK_C

#include "sleepk.h"

static inline uint32_t read_cycles(void) {
    uint32_t cycles;
    __asm__ __volatile__(
        "rdcycle %0\n"
        : "=r"(cycles)
    );
    return cycles;
}

#define CPU_FREQ_HZ 2000000000  // 2000 MHz

void sleepk_ms(uint32_t ms) {
    uint32_t cycles_per_ms = CPU_FREQ_HZ / 1000;
    uint32_t total_cycles = cycles_per_ms * ms;
    uint32_t start = read_cycles();
    
    while ((read_cycles() - start) < total_cycles) {
        __asm__ __volatile__("nop");
    }
}

#endif
