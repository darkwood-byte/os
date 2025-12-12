#ifndef PTE_UTILS_H
#define PTE_UTILS_H

#include "types.h"
#include "pte.h"

// Helper macros voor VPN extractie volgens Sv32
#define VPN1_FROM_VPA(vpa) ((vpa) >> 22)              // Bits 31:22
#define VPN0_FROM_VPA(vpa) (((vpa) >> 12) & 0x3FF)    // Bits 21:12
#define OFFSET_FROM_VPA(vpa) ((vpa) & 0xFFF)          // Bits 11:0

// Helper voor PPN extractie uit PFA (Page Frame Address)
#define PPN_FROM_PFA(pfa) ((pfa) >> 12)               // Haal PPN uit PFA (shift 12 bits)

// Controleer of een PDE geldig is
#define PDE_IS_VALID(pde) ((pde) & PTE_FLG_V)

// Haal PPN uit een PDE/PTE (22 meest linkse bits)
#define PPN_FROM_ENTRY(entry) ((entry) >> 10)

// Maak PFA van PPN
#define PFA_FROM_PPN(ppn) ((ppn) << 12)

#endif // PTE_UTILS_H
