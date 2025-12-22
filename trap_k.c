#include "trap_k.h"

void handle_trap(trap_frame *tf) {
    // Read traprelated CSRs
    uint32_t csr_sepc = read_csr(SEPC);
    uint32_t csr_scause = read_csr(SCAUSE);
    uint32_t csr_stval = read_csr(STVAL);
    
    k_printf("Trap detected! scause=%x, stval=%x, sepc=%p\n", 
             csr_scause, csr_stval, csr_sepc);
    k_printf("Trap remains unhandled as of now. Kernel might become wonky! Consida' yo'ass warn'd!\n");
    
    //skip the broken instruction
    csr_sepc += 4;
    write_csr(sepc, csr_sepc);
    k_panic("debug\n","");
    (void)tf; // Suppress unused parameter warning for now hehehehehehe
}

__attribute__((naked))
__attribute__((aligned(4)))
void switch_trap(void) {
    __asm__ __volatile__(
        "csrrw sp, sscratch, sp\n"

        "addi sp, sp, -124\n"
        
        STORE_REGS()
        

        "csrr a0, sscratch\n"
        "sw a0, 124(sp)\n"
        
        "addi a0, sp, 128\n"  // 124 + 4 = 128 (begin van de stack frame)
        "csrw sscratch, a0\n"
       
        "mv a0, sp\n"         
        "call handle_trap\n"
        
        RESTORE_REGS()
        
        // Herstel sp
        "lw sp, 124(sp)\n"
        
        "sret\n"
    );
}
