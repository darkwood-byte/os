#include "program_k.h"

__attribute__((naked))
void switch_proc(uint32_t *current_psp, uint32_t *next_psp) {
    __asm__ __volatile__(
        // Save registers op huidige stack
        "addi sp, sp, -52\n"
        "sw ra, 0(sp)\n"
        "sw s0, 4(sp)\n"
        "sw s1, 8(sp)\n"
        "sw s2, 12(sp)\n"
        "sw s3, 16(sp)\n"
        "sw s4, 20(sp)\n"
        "sw s5, 24(sp)\n"
        "sw s6, 28(sp)\n"
        "sw s7, 32(sp)\n"
        "sw s8, 36(sp)\n"
        "sw s9, 40(sp)\n"
        "sw s10, 44(sp)\n"
        "sw s11, 48(sp)\n"
        
        // Switch stack pointer
        // *current_psp = sp
        "sw sp, 0(a0)\n"
        // sp = *next_psp
        "lw sp, 0(a1)\n"
        
        // Restore registers van nieuwe stack
        "lw ra, 0(sp)\n"
        "lw s0, 4(sp)\n"
        "lw s1, 8(sp)\n"
        "lw s2, 12(sp)\n"
        "lw s3, 16(sp)\n"
        "lw s4, 20(sp)\n"
        "lw s5, 24(sp)\n"
        "lw s6, 28(sp)\n"
        "lw s7, 32(sp)\n"
        "lw s8, 36(sp)\n"
        "lw s9, 40(sp)\n"
        "lw s10, 44(sp)\n"
        "lw s11, 48(sp)\n"
        "addi sp, sp, 52\n"
        
        "ret\n"
    );
}

void Flush_TLB(pcb* nextproc){
    uint32_t pdbr_value = (uint32_t)(uintptr_t)nextproc->pdbr;
    uint32_t ppn = (pdbr_value >> 12) & 0x003FFFFF;  // Extract PPN (bits 21:0)
    uint32_t satp_value = SV32_MMU_ON | ppn;          // MODE=1 + PPN

    __asm__ __volatile__(
        "csrw satp, %0\n"
        "sfence.vma\n"
        :
        : "r" (satp_value)
    );
}

void yield(void) {
    pcb *oldproc = currproc;
    
    // Update state van huidig proces
    if (currproc && currproc->pstate == RUNNING) {
        currproc->pstate = READY;
    }
    
    // Zoek volgende READY proces (skip idle in eerste instantie)
    uint32_t start_pid = currproc ? currproc->pid + 1 : 1;
    pcb *nextproc = NULL;
    
    // Eerste ronde: zoek READY processen, SKIP PID 0
    for (uint32_t i = 0; i < MAXPROCS; i++) {
        uint32_t check_pid = (start_pid + i) % MAXPROCS;
        
        if (check_pid == 0) {
            continue;  // Skip idle
        }
        
        if (proclist[check_pid].pstate == READY) {
            nextproc = &proclist[check_pid];
            break;
        }
    }
    
    // Als niks gevonden, check of huidig proces zelf READY is
    if (!nextproc && currproc && currproc->pstate == READY) {
        nextproc = currproc;
    }
    
    // Als GEEN ENKEL proces READY is, val terug naar idle (PID 0) oftwel kernel
    if (!nextproc) {
        nextproc = idleproc;
    }
    
    // Als hetzelfde proces geen switch nodig anders super veel bugs
    if (nextproc == oldproc) {
        return;
    }
    
    // NIEUW: Zet kernel stack pointer in sscratch voor nextproc (volgens lesdoc p15)
    uint32_t kernel_stack_top = (uint32_t)&nextproc->pstack[sizeof(nextproc->pstack)];
    
    __asm__ __volatile__(
        "csrw sscratch, %0\n"
        :
        : "r" (kernel_stack_top)
    );
    
    // Update currproc
    currproc = nextproc;
    currproc->pstate = RUNNING;
    
    // Flush TLB en set SATPset met nieuwe PDBR 
    Flush_TLB(nextproc);
    
    // Context switch
    switch_proc(&oldproc->psp, &nextproc->psp);
}
