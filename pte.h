// pte.h of types.h
#ifndef PTE_H
#define PTE_H

#include "pte_utils.h"

// SATP register flags
#define SV32_MMU_ON (1U << 31)  // MODE=1 voor Sv32 (bit 31)

// Page Table Entry flags (bits 9:0)
#define PTE_FLG_V (1 << 0)  // Valid
#define PTE_FLG_R (1 << 1)  // Readable
#define PTE_FLG_W (1 << 2)  // Writable
#define PTE_FLG_X (1 << 3)  // Executable
#define PTE_FLG_U (1 << 4)  // User
#define PTE_FLG_G (1 << 5)  // Global
#define PTE_FLG_A (1 << 6)  // Accessed
#define PTE_FLG_D (1 << 7)  // Dirty

// Reserved for Software bits (bits 9:8) - kunnen voor OS-doeleinden gebruikt worden
#define PTE_FLG_RSW0 (0 << 8)  // Standaard 0
#define PTE_FLG_RSW1 (0 << 9)  // Standaard 0

#endif // PTE_H
