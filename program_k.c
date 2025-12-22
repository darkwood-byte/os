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
    // Save old currproc voor switch_proc
    k_printf("DEBUG: Entering yield()\n");
    k_printf("  currproc PID = %d\n", currproc ? currproc->pid : -1);
    
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
    
    // DEBUG: Print belangrijke informatie
    k_printf("  Switching from PID %d to PID %d\n", 
             oldproc ? oldproc->pid : -1, nextproc->pid);
    
    k_printf("  oldproc->psp = ");
    if (oldproc && oldproc->psp) {
        k_printf("%x\n", oldproc->psp);
    } else {
        k_printf("NULL\n");
    }
    
    k_printf("  nextproc->psp = ");
    if (nextproc->psp) {
        k_printf("%x\n", nextproc->psp);
        
        // Dump stack inhoud
        uint32_t *stack = (uint32_t *)nextproc->psp;
        k_printf("  Stack dump (first 16 words at psp):\n");
        for (int i = -8; i < 8; i++) {
            uint32_t addr = (uint32_t)(&stack[i]);
            uint32_t value = stack[i];
            
            // Print zonder 0x prefix zoals gevraagd
            k_printf("    [");
            if (i < 0) {
                k_printf("-");
                k_printf("%x", -i*4);
            } else {
                k_printf("+");
                k_printf("%x", i*4);
            }
            k_printf("] ");
            
            k_printf("%x: ", addr);
            k_printf("%x", value);
            
            // Mark special values
            if (value == (uint32_t)switch_umode) {
                k_printf(" <-- switch_umode");
            } else if (value == (uint32_t)switch_proc) {
                k_printf(" <-- switch_proc");
            } else if (value == (uint32_t)yield) {
                k_printf(" <-- yield");
            } else if (value == 0) {
                k_printf(" <-- zero");
            }
            k_printf("\n");
        }
    } else {
        k_printf("NULL\n");
    }
    
    k_printf("  switch_umode address = %x\n", (uint32_t)switch_umode);
    k_printf("  nextproc->pdbr = %x\n", nextproc->pdbr);
    
    // NIEUW: Zet kernel stack pointer in sscratch voor nextproc (volgens lesdoc p15)
    uint32_t kernel_stack_top = (uint32_t)&nextproc->pstack[sizeof(nextproc->pstack)];
    k_printf("  Setting sscratch to kernel_stack_top = %x\n", kernel_stack_top);
    
    __asm__ __volatile__(
        "csrw sscratch, %0\n"
        :
        : "r" (kernel_stack_top)
    );
    
    // Update currproc
    currproc = nextproc;
    currproc->pstate = RUNNING;

    k_printf("  Flushing TLB with pdbr = %x\n", nextproc->pdbr);
    
    // Flush TLB en set SATPset met nieuwe PDBR 
    Flush_TLB(nextproc);

    k_printf("  Calling switch_proc:\n");
    k_printf("    &oldproc->psp = %x (value = ", &oldproc->psp);
    if (oldproc) {
        k_printf("%x", oldproc->psp);
    } else {
        k_printf("NULL");
    }
    k_printf(")\n");
    
    k_printf("    &nextproc->psp = %x (value = %x)\n", 
             &nextproc->psp, nextproc->psp);
    
    // Context switch
    switch_proc(&oldproc->psp, &nextproc->psp);
    
    // Deze code wordt alleen bereikt na terugkeer van switch_proc
    k_printf("DEBUG: Returned from switch_proc to PID %d\n", currproc->pid);
}
