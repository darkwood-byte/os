#ifndef CSR_H
#define CSR_H

#include "types.h"

typedef enum {
    // Supervisor Trap Setup
    SSTATUS = 0x100,      // Supervisor status register
    SIE = 0x104,          // Supervisor interrupt-enable register
    STVEC = 0x105,        // Supervisor trap handler base address
    SCOUNTEREN = 0x106,   // Supervisor counter enable
    
    // Supervisor Configuration
    SENVCFG = 0x10A,      // Supervisor environment configuration register
    
    // Supervisor Trap Handling
    SSCRATCH = 0x140,     // Scratch register for supervisor trap handlers
    SEPC = 0x141,         // Supervisor exception program counter
    SCAUSE = 0x142,       // Supervisor trap cause
    STVAL = 0x143,        // Supervisor bad address or instruction
    SIP = 0x144,          // Supervisor interrupt pending
    
    // Supervisor Protection and Translation
    SATP = 0x180,         // Supervisor address translation and protection
    
    // Supervisor State Enable Registers
    SSTATEEN0 = 0x10C,
    SSTATEEN1 = 0x10D,
    SSTATEEN2 = 0x10E,
    SSTATEEN3 = 0x10F,
    
    // Debug/Trace Registers (Supervisor Read-Only)
    SCONTEXT = 0x5A8,     // Supervisor-mode context register
    
    // Supervisor Counter/Timers (Read-Only)
    SCYCLE = 0xC00,       // Supervisor cycle counter
    STIME = 0xC01,        // Supervisor time counter
    SINSTRET = 0xC02,     // Supervisor instructions-retired counter
    
    // Example of a Read-Only CSR (for testing illegal writes)
    SCOUNTOVF = 0xDA0     // Supervisor count overflow (SRO - Read Only)
} csr_register;


#define write_csr(reg, val) \
    do { \
        __asm__ __volatile__("csrw " #reg ", %0" :: "r"((uint32_t)(val))); \
    } while(0)


static inline uint32_t read_csr(csr_register reg) {
    uint32_t value;
    __asm__ __volatile__("csrr %0, %1" : "=r"(value) : "i"(reg));
    return value;
}

static inline uint32_t read_csr_num(uint32_t reg_num) {
    uint32_t value;
    switch(reg_num) {
        case 0x105: __asm__ volatile("csrr %0, 0x105" : "=r"(value)); break;
        case 0x140: __asm__ volatile("csrr %0, 0x140" : "=r"(value)); break;
        case 0x141: __asm__ volatile("csrr %0, 0x141" : "=r"(value)); break;
        case 0x142: __asm__ volatile("csrr %0, 0x142" : "=r"(value)); break;
        case 0x143: __asm__ volatile("csrr %0, 0x143" : "=r"(value)); break;
        default: value = 0; break;
    }
    return value;
}

#endif // CSR_H
