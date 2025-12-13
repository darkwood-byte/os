#ifndef PTE_H
#define PTE_H

#include "pte_utils.h"

// SATP register flags
#define SV32_MMU_ON (1U << 31)  // MODE=1 voor Sv32 (bit 31) anders doet lekerlijk niks het

// Page Table Entry flags lijst (bits 9:0)
#define PTE_FLG_V (1 << 0)  // Valid
#define PTE_FLG_R (1 << 1)  // Readable
#define PTE_FLG_W (1 << 2)  // Writable
#define PTE_FLG_X (1 << 3)  // Executable
#define PTE_FLG_U (1 << 4)  // User
#define PTE_FLG_G (1 << 5)  // Global
#define PTE_FLG_A (1 << 6)  // Accessed
#define PTE_FLG_D (1 << 7)  // Dirty

#endif // PTE_H
