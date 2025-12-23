#include "trap_k.h"

#define SYSCALL_PUTCHAR 0x01
#define SYSCALL_GETCHAR 0x02
#define SYSCALL_EXIT    0x03
#define SYSCALL_YIELD    0x04
#define SYSCALL_KILL 0x05

void handle_syscall(trap_frame *tf) {
    uint32_t syscall_num = tf->a3;  // Syscall nummer staat in a3 volgens je wrapper
    uint32_t arg0 = tf->a0;
    uint32_t arg1 = tf->a1;
    uint32_t arg2 = tf->a2;
    
    switch(syscall_num) {
        case SYSCALL_PUTCHAR:
            k_putchar((char)arg0);
            tf->a0 = 0;  // Return value
            break;
            
        case SYSCALL_GETCHAR:
            tf->a0 = k_readchar();
            break;
            
        case SYSCALL_EXIT:
            currproc->pstate = BLOCKED;
            yield();
            break;
        case SYSCALL_YIELD:
            currproc->pstate = READY;
            yield();
            break;
        case SYSCALL_KILL:
            currproc->pstate = NOPROC;
            yield();
            break;
            
        default:
            k_printf("Unknown syscall: %d\n", syscall_num);
            tf->a0 = 0xFFFFFFFF;;  // Error return
            break;
    }
    (void)arg1;
    (void)arg2;
}

void handle_trap(trap_frame *tf) {
    // Read traprelated CSRs
    uint32_t csr_sepc = read_csr(SEPC);
    uint32_t csr_scause = read_csr(SCAUSE);
    uint32_t csr_stval = read_csr(STVAL);
    
   
    if (csr_scause == 0x08){
        handle_syscall(tf);
    }
    else{
        k_printf("Unknow trap detected! scause=%x, stval=%x, sepc=%p\n", 
        csr_scause, csr_stval, csr_sepc);
         k_printf("Trap remains unhandled as of now. Kernel might become wonky! Consida' yo'ass warn'd!\n");
    }
    //skip the broken instruction
    csr_sepc += 4;
    write_csr(sepc, csr_sepc);
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
        
        "addi a0, sp, 124\n"  //<--- oude bug(FIXED be aware)
        "csrw sscratch, a0\n"
       
        "mv a0, sp\n"         
        "call handle_trap\n"
        
        RESTORE_REGS()
        
        // Herstel sp
        "lw sp, 124(sp)\n"
        
        "sret\n"
    );
}
