#include "program_k.h"
#include "print_k.h"

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
    // DEBUG: Check for NULL pointers
    if (nextproc == NULL) {
        k_panic("[DEBUG] Flush_TLB: NULL nextproc!\n", "");
        return;
    }
    
    // DEBUG: Check if pdbr is valid
    if (nextproc->pdbr_phys == 0) {
        k_panic("[DEBUG] Flush_TLB: PID %d has invalid pdbr_phys=0x%x!\n", 
                nextproc->pid, nextproc->pdbr_phys);
        return;
    }
    
    // DEBUG: Log the switch
    k_printf("[DEBUG] Flush_TLB: Switching to PID %d, pdbr_phys=0x%x, pdbr_virt=%p\n",
             nextproc->pid, nextproc->pdbr_phys, nextproc->pdbr_virt);
    
    // Calculate SATP value - MUST use PHYSICAL address!
    uint32_t ppn = PPN_FROM_PFA(nextproc->pdbr_phys);
    uint32_t satp_value = SV32_MMU_ON | ppn;
    
    // DEBUG: Log SATP value
    k_printf("[DEBUG] Flush_TLB: Setting SATP=0x%x (ppn=0x%x from phys 0x%x)\n", 
             satp_value, ppn, nextproc->pdbr_phys);
    
    __asm__ __volatile__(
        "csrw satp, %0\n"
        "sfence.vma\n"
        :
        : "r" (satp_value)
    );
}

void k_sp(void){
    k_printf("\n====active pcb's====\n");
    for (uint32_t i = 0; i < MAXPROCS; i++){
        if(proclist[i].pstate == NOPROC){
            k_printf("p: %d :: does not exist\n", i);
        }
        else{
            k_printf("p: %d :: state : %d  psp : %p  pdbr_phys: 0x%x  pdbr_virt: %p\n", 
                     i, proclist[i].pstate, proclist[i].psp, 
                     proclist[i].pdbr_phys, proclist[i].pdbr_virt);
        }
    }
    k_printf("\n====end of active pcb's====\n");
}

void yield(void) {
    pcb *oldproc = currproc;
    
    // DEBUG: Log yield start
    k_printf("[DEBUG] yield: Called from PID %d\n", oldproc ? oldproc->pid : -1);
    
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
            k_printf("[DEBUG] yield: Found READY process PID %d\n", check_pid);
            break;
        }
    }
    
    // Als niks gevonden, check of huidig proces zelf READY is
    if (!nextproc && currproc && currproc->pstate == READY) {
        nextproc = currproc;
        k_printf("[DEBUG] yield: Reusing current process PID %d\n", currproc->pid);
    }
    
    // Als GEEN ENKEL proces READY is, val terug naar idle (PID 0) oftwel kernel
    if (!nextproc) {
        nextproc = idleproc;
        k_printf("[DEBUG] yield: Falling back to idle (PID 0)\n");
    }
    
    // Als hetzelfde proces geen switch nodig anders super veel bugs
    if (nextproc == oldproc) {
        k_printf("[DEBUG] yield: Same process, no switch needed\n");
        return;
    }
    
    // DEBUG: Log the switch
    k_printf("[DEBUG] yield: Switching from PID %d to PID %d\n", 
             oldproc ? oldproc->pid : -1, nextproc->pid);
    
    // Zet kernel stack pointer in sscratch voor nextproc
    uint32_t kernel_stack_top = (uint32_t)&nextproc->pstack[sizeof(nextproc->pstack)];
    
    __asm__ __volatile__(
        "csrw sscratch, %0\n"
        :
        : "r" (kernel_stack_top)
    );

    // Update currproc
    currproc = nextproc;
    currproc->pstate = RUNNING;
    
    // Flush TLB en set SATP met nieuwe PDBR 
    Flush_TLB(nextproc);
    
    // DEBUG: Before context switch
    k_printf("[DEBUG] yield: Before switch - oldpsp=%p, newpsp=%p\n", 
             oldproc ? oldproc->psp : 0, nextproc->psp);
    
    // Context switch
    switch_proc(oldproc ? &oldproc->psp : NULL, &nextproc->psp);
}
