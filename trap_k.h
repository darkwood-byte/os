#ifndef TRAP_H
#define TRAP_H

#include "trap_frame_k.h"
#include "types.h"
#include "csr.h"
#include "print_k.h"
#include "program_k.h"

void handle_trap(trap_frame *tf) ;

#define RESTORE_REGS()              \
    "lw ra,   0(sp)\n"              \
    "lw gp,   8(sp)\n"              \
    "lw tp,  12(sp)\n"              \
    "lw t0,  16(sp)\n"              \
    "lw t1,  20(sp)\n"              \
    "lw t2,  24(sp)\n"              \
    "lw s0,  28(sp)\n"              \
    "lw s1,  32(sp)\n"              \
    "lw a0,  36(sp)\n"              \
    "lw a1,  40(sp)\n"              \
    "lw a2,  44(sp)\n"              \
    "lw a3,  48(sp)\n"              \
    "lw a4,  52(sp)\n"              \
    "lw a5,  56(sp)\n"              \
    "lw a6,  60(sp)\n"              \
    "lw a7,  64(sp)\n"              \
    "lw s2,  68(sp)\n"              \
    "lw s3,  72(sp)\n"              \
    "lw s4,  76(sp)\n"              \
    "lw s5,  80(sp)\n"              \
    "lw s6,  84(sp)\n"              \
    "lw s7,  88(sp)\n"              \
    "lw s8,  92(sp)\n"              \
    "lw s9,  96(sp)\n"              \
    "lw s10, 100(sp)\n"             \
    "lw s11, 104(sp)\n"             \
    "lw t3, 108(sp)\n"              \
    "lw t4, 112(sp)\n"              \
    "lw t5, 116(sp)\n"              \
    "lw t6, 120(sp)\n"

    #define STORE_REGS()               \
    "sw ra,   0(sp)\n"             \
    "sw gp,   8(sp)\n"             \
    "sw tp,  12(sp)\n"             \
    "sw t0,  16(sp)\n"             \
    "sw t1,  20(sp)\n"             \
    "sw t2,  24(sp)\n"             \
    "sw s0,  28(sp)\n"             \
    "sw s1,  32(sp)\n"             \
    "sw a0,  36(sp)\n"             \
    "sw a1,  40(sp)\n"             \
    "sw a2,  44(sp)\n"             \
    "sw a3,  48(sp)\n"             \
    "sw a4,  52(sp)\n"             \
    "sw a5,  56(sp)\n"             \
    "sw a6,  60(sp)\n"             \
    "sw a7,  64(sp)\n"             \
    "sw s2,  68(sp)\n"             \
    "sw s3,  72(sp)\n"             \
    "sw s4,  76(sp)\n"             \
    "sw s5,  80(sp)\n"             \
    "sw s6,  84(sp)\n"             \
    "sw s7,  88(sp)\n"             \
    "sw s8,  92(sp)\n"             \
    "sw s9,  96(sp)\n"             \
    "sw s10, 100(sp)\n"            \
    "sw s11, 104(sp)\n"            \
    "sw t3, 108(sp)\n"             \
    "sw t4, 112(sp)\n"             \
    "sw t5, 116(sp)\n"             \
    "sw t6, 120(sp)\n"

void switch_trap(void) ;


#endif
