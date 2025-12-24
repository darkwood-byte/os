#include "trap_k.h"

#define SYSCALL_PUTCHAR 0x01
#define SYSCALL_GETCHAR 0x02
#define SYSCALL_EXIT    0x03
#define SYSCALL_YIELD    0x04
#define SYSCALL_KILL 0x05
#define SYSCALL_START 0x06
#define SYSCALL_SR_READ 0x07
#define SYSCALL_SR_WRITE 0x08
#define SYSCALL_S_READ 0x0A
#define SYSCALL_S_WRITE 0x0B

void handle_syscall(trap_frame *tf) {
    uint32_t syscall_num = tf->a3;  // Syscall nummer
    uint32_t arg0 = tf->a0;
    uint32_t arg1 = tf->a1;
    uint32_t arg2 = tf->a2;
    
    switch(syscall_num) {
        //writing
        case SYSCALL_PUTCHAR:
            k_putchar((char)arg0);
            tf->a0 = 0;  // Return value
            break;
            
        case SYSCALL_GETCHAR:
            tf->a0 = k_readchar();
            break;
        //apps
        case SYSCALL_EXIT:
            currproc->pstate = BLOCKED;
            yield();
            break;
        case SYSCALL_YIELD:
            currproc->pstate = READY;
            yield();
            break;
        case SYSCALL_KILL:
            if(arg0 == 0){//when you kill the kernel the kernel kill you =]
                currproc->pstate = NOPROC;
                yield();
            }
            else{
                if (arg0 > MAXPROCS)k_panic("system call tried to call non valid proces id: %d\n", arg0);
                if(proclist[arg0].parent_id == 0 && currproc->parent_id != 0)k_panic("can't stop a root program from non root program id: \n", arg0);//0 means that the kernel made the program
                else proclist[arg0].pstate = NOPROC;
            }
            break;
        case SYSCALL_START:  
            tf->a0 = start_app(arg0);
            break;
        //sockets
        case SYSCALL_SR_READ:  
            tf->a0 = read_rsocket(arg0);
            break;
        case SYSCALL_SR_WRITE:  
            write_rsocket(arg0, arg1);
            break;
        case SYSCALL_S_READ:  
            tf->a0 = read_socket(arg0, arg1);
            break;
        case SYSCALL_S_WRITE:  
            write_socket(arg0, arg1, arg2);
            break;
        //errors
        default:
            k_printf("Unknown syscall: %d\n", syscall_num);
            tf->a0 = 0xFFFFFFFF;;  // Error return
            break;
    }
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
