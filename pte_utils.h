#ifndef PTE_UTILS_H
#define PTE_UTILS_H

#include "types.h"
#include "pte.h"

// Helper macro voor VPN extractie volgens Sv32 link
#define VPN1_FROM_VPA(vpa) ((vpa) >> 22)              // B31:22
#define VPN0_FROM_VPA(vpa) (((vpa) >> 12) & 0x3FF)    // B21:12
#define OFFSET_FROM_VPA(vpa) ((vpa) & 0xFFF)          // B11:0

// helper voor PPN extractie uit PFAPage frame Addres
#define PPN_FROM_PFA(pfa) ((pfa) >> 12)          

// Controleer of een PDE geldig is
#define PDE_IS_VALID(pde) ((pde) & PTE_FLG_V)

// Haal PPN uit een PDE/PTE (22 meest linkse bits)
#define PPN_FROM_ENTRY(entry) ((entry) >> 10)

// Maak PFA van PPN
#define PFA_FROM_PPN(ppn) ((ppn) << 12)

#endif // PTE_UTILSH
