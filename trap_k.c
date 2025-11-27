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
    
    (void)tf; // Suppress unused parameter warning for now hehehehehehe
}

__attribute__((naked))
__attribute__((aligned(4)))
void switch_trap(void) {
    __asm__ __volatile__(
        // save the sp to sscratch temp register
        "csrw sscratch, sp\n"
        
        //Reserve 124 bytes  (31 * 4 = 124)
        "addi sp, sp, -124\n"
        
       STORE_REGS()
        
        "csrr a0, sscratch\n" // put the old sp back for handle trap function
        "sw a0, 4(sp)\n"      
        //call the handle function with the stack pointer as argument to lead to the struct.
        "mv a0, sp\n"         // First argument: trap_frame *tf
        "call handle_trap\n"
        
        RESTORE_REGS()
        
        // Restore sp
        "lw sp, 4(sp)\n"     
        
        // return
        "sret\n"
    );
}
