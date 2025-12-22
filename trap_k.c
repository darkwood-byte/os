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
        // ===============================
        // VERVANG het oude: "csrw sscratch, sp\n"
        // MET het nieuwe: wissel sp en sscratch (volgens lesdoc p15-16)
        "csrrw sp, sscratch, sp\n"
        // ===============================
        
        // Reserveer 124 bytes (31 * 4 = 124)
        "addi sp, sp, -124\n"
        
        STORE_REGS()
        
        // Lees de oude sp (nu in sscratch) en sla die op
        "csrr a0, sscratch\n"
        "sw a0, 124(sp)\n"
        
        // ===============================
        // NIEUW: Reset sscratch naar kernel stack top (volgens lesdoc p16)
        "addi a0, sp, 128\n"  // 124 + 4 = 128 (begin van de stack frame)
        "csrw sscratch, a0\n"
        // ===============================
        
        // Roep handle_trap aan met stack pointer als argument
        "mv a0, sp\n"         // First argument: trap_frame *tf
        "call handle_trap\n"
        
        RESTORE_REGS()
        
        // Herstel sp
        "lw sp, 124(sp)\n"
        
        // Keer terug
        "sret\n"
    );
}
