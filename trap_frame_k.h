#ifndef TRAPFRAME_H
#define TRAPFRAME_H

#include "types.h"



typedef uint32_t pframe_addr_t;

typedef struct __attribute__((packed)) {
    uint32_t ra;   // x1  - return address
    uint32_t sp;   // x2  - stack pointer
    uint32_t gp;   // x3  - global pointer
    uint32_t tp;   // x4  - thread pointer
    uint32_t t0;   // x5  - temporary 0
    uint32_t t1;   // x6  - temporary 1
    uint32_t t2;   // x7  - temporary 2
    uint32_t s0;   // x8  - saved register 0 / frame pointer
    uint32_t s1;   // x9  - saved register 1
    uint32_t a0;   // x10 - argument 0 / return value 0
    uint32_t a1;   // x11 - argument 1 / return value 1
    uint32_t a2;   // x12 - argument 2
    uint32_t a3;   // x13 - argument 3
    uint32_t a4;   // x14 - argument 4
    uint32_t a5;   // x15 - argument 5
    uint32_t a6;   // x16 - argument 6
    uint32_t a7;   // x17 - argument 7
    uint32_t s2;   // x18 - saved register 2
    uint32_t s3;   // x19 - saved register 3
    uint32_t s4;   // x20 - saved register 4
    uint32_t s5;   // x21 - saved register 5
    uint32_t s6;   // x22 - saved register 6
    uint32_t s7;   // x23 - saved register 7
    uint32_t s8;   // x24 - saved register 8
    uint32_t s9;   // x25 - saved register 9
    uint32_t s10;  // x26 - saved register 10
    uint32_t s11;  // x27 - saved register 11
    uint32_t t3;   // x28 - temporary 3
    uint32_t t4;   // x29 - temporary 4
    uint32_t t5;   // x30 - temporary 5
    uint32_t t6;   // x31 - temporary 6
} trap_frame;

#endif
