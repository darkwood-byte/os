#ifndef VARIADIC_H
#define VARIADIC_H
#include "types.h"

typedef struct {
    uint32_t gp_regs[7];     // a1-a7 argumenten
    uint32_t *stack_ptr;     // pointer naar stack argumenten  
    uint32_t index;          // huidige argument index
} va_list_rv;

#define va_start_rv(ap, last)                        \
    do {                                             \
        __asm__ volatile (                           \
            "sw a1, 0(%0)\n\t"                       \
            "sw a2, 4(%0)\n\t"                       \
            "sw a3, 8(%0)\n\t"                       \
            "sw a4, 12(%0)\n\t"                      \
            "sw a5, 16(%0)\n\t"                      \
            "sw a6, 20(%0)\n\t"                      \
            "sw a7, 24(%0)\n\t"                      \
            "addi t0, s0, 32\n\t"                    \
            "sw t0, 28(%0)\n\t"                      \
            "sw zero, 32(%0)\n\t"                    \
            :                                        \
            : "r"(&(ap))                             \
            : "t0", "memory"                         \
        );                                           \
    } while (0)

// Helper functie voor va_arg_rv anders werkt de stack niet grrrrrrrr.
static inline uint32_t __va_arg_rv_impl(va_list_rv *ap) {
    uint32_t result;
    if (ap->index < 7) {
        result = ap->gp_regs[ap->index];
    } else {
        result = ap->stack_ptr[ap->index - 7];
    }
    ap->index++;
    return result;
}

#define va_arg_rv(ap, type) ((type)__va_arg_rv_impl(&(ap)))

#define va_end_rv(ap)                                \
    do {                                             \
        (ap).index = 0;                              \
    } while (0)

#endif 
