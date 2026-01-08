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
#define SYSCALL_SR_CLAIM 0x0C
#define SYSCALL_SR_UNCLAIM 0x0D
#define SYSCALL_S_CLAIM 0x0E
#define SYSCALL_S_UNCLAIM 0x0F
#define SYSCALL_SR_INFO 0x10
#define SYSCALL_S_INFO 0x11
#define SYSCALL_SR_FIND 0x12
#define SYSCALL_S_FIND 0x13
#define SYSCALL_GET_APP_NAME 0x14
#define SYSCALL_GET_APP_ID 0x15
#define SYSCALL_SHUTDOWN 0x16
#define SYSCALL_GETPCB 0x17

#include "program_k.h"

static void uint32_to_chars(uint32_t value, char buffer[5]) {
    buffer[0] = (char)((value >> 24) & 0xFF);
    buffer[1] = (char)((value >> 16) & 0xFF);
    buffer[2] = (char)((value >> 8) & 0xFF);
    buffer[3] = (char)(value & 0xFF);
    buffer[4] = '\0';
}

void handle_syscall(trap_frame *tf) {
    uint32_t syscall_num = tf->a3;
    uint32_t arg0 = tf->a0;
    uint32_t arg1 = tf->a1;
    uint32_t arg2 = tf->a2;
    
    switch(syscall_num) {
        case SYSCALL_PUTCHAR:
            k_putchar((char)arg0);
            tf->a0 = 0;
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
            if(arg0 == 0 || arg0 == currproc->pid) {
                free_proc(currproc);
                currproc->pstate = NOPROC;
                yield();
            } else {
                if (arg0 > MAXPROCS) k_panic("system call tried to call non valid proces id: %d\n", arg0);
                if(proclist[arg0].parent_id == 0 && currproc->parent_id != 0) {
                    k_panic("can't stop a root program from non root program id: \n", arg0);
                } else {
                    free_proc(&proclist[arg0]);
                }
            }
            break;

        case SYSCALL_START:  
            tf->a0 = start_app(arg0);
            break;

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
        case SYSCALL_SR_CLAIM:  
            claim_rsocket(arg0, arg1);
            break;
        case SYSCALL_S_CLAIM:  
            claim_socket(arg0, arg1);
            break;
        case SYSCALL_SR_UNCLAIM:  
            unclaim_rsocket(arg0);
            break;
        case SYSCALL_S_UNCLAIM:  
            unclaim_socket(arg0);
            break;
         case SYSCALL_SR_INFO:  
            tf->a0 = info_rsocket(arg0);
            break;
        case SYSCALL_S_INFO:  
            tf->a0 = info_socket(arg0);
            break;
        case SYSCALL_SR_FIND:  
            tf->a0 = find_free_rsocket(arg0);
            break;
        case SYSCALL_S_FIND:  
            tf->a0 = find_free_socket(arg0);
            break;
        case SYSCALL_GET_APP_ID: { 
            char temp[5] = {' '};
            uint32_to_chars(arg0, temp);
            tf->a0 = get_app_id(temp);
            break;}
        case SYSCALL_GET_APP_NAME:  
            tf->a0 = get_app_name(arg0);
            break;
         case SYSCALL_SHUTDOWN:  
            exit(arg0);
            break;
         case SYSCALL_GETPCB:  
            tf->a0  = currproc->pid;
            break;
        default:
            k_printf("Unknown syscall: %d\n", syscall_num);
            tf->a0 = 0xFFFFFFFF;;
            break;
    }
}

void handle_trap(trap_frame *tf) {
    uint32_t csr_sepc = read_csr(SEPC);
    uint32_t csr_scause = read_csr(SCAUSE);
    uint32_t csr_stval = read_csr(STVAL);
    
   
    if (csr_scause == 0x08){
        handle_syscall(tf);
    }
    else{
        k_printf("Unknow trap detected! scause=%x, stval=%x, sepc=%p\n", 
        csr_scause, csr_stval, csr_sepc);
         k_panic("Trap remains unhandled as of now. Kernel might become wonky! Consida' yo'ass warn'd!\n", "");
    }

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
        
        "addi a0, sp, 124\n"
        "csrw sscratch, a0\n"
       
        "mv a0, sp\n"
        "call handle_trap\n"
        
        RESTORE_REGS()
        
        "lw sp, 124(sp)\n"
        
        "sret\n"
    );
}
