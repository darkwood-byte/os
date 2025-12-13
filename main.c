#include "main.h"

extern char __bss[], __bss_end[], __stack_top[];
extern char __free_ram_start[], __free_ram_end[];
extern char __kernel_base[];

void kernel_bootstrap(void){
    k_printf("\nBoot done. . .\n");

     k_printf("\n=== Kernel Boot ===\n");
    memset(__bss, 0, ((size_t)__bss_end - (size_t)__bss));
    k_printf("BSS initialized\n");
    
    write_csr(stvec, (uint32_t)switch_trap);
    k_printf("Trap handler registered at: %p\n", (uint32_t)switch_trap);

    init_memory();
    k_printf("Free RAM: %p - %p\n", (uint32_t)__free_ram_start, (uint32_t)__free_ram_end);
    
    k_printf("\nMaking kernel idle process pcb:\n\n");
    idleproc = spawn_proc((uint32_t)NULL);
    currproc = idleproc;
    
    k_printf("\nKernel Boot done. . .\n");
}

void k_sleep(uint32_t ms) {
    const uint32_t FACTOR = 200000;//0.5 sec op mijn laptop maar dat vind ik juist lekker snell
    const uint32_t MAX_MS = 0xffffffff / FACTOR;
    
    if (ms > MAX_MS) {
        ms = MAX_MS;  // Capie 
    }
    
    uint32_t total = ms * FACTOR;
    for (uint32_t i = 0; i < total; i++) {
        __asm__ __volatile__ ("nop");
    }
}

void k_sp(void){//print de pcb status
    k_printf("\n====active pcb's====\n");
    for (uint32_t i = 0; i < MAXPROCS; i++){
        if(proclist[i].pstate == NOPROC){
            k_printf("p: %d :: does not exist\n", i);
        }
        else{
            k_printf("p: %d :: state : %d  psp : %p\n", i, proclist[i].pstate, proclist[i].psp);
            k_printf("&proclist[%d] = %p\n", i, &proclist[i]);

        }
    }
    k_printf("\n====end of active pcb's====\n");
}

// === gefixde TEST PROCESSEN ;)===
void proc0(void) {
    k_printf("\nStart van proc0 op PID: %d\n", currproc->pid);
     for (uint8_t i = 0; i < 8; i++){
        k_printf("currently in loop %d from proc0 hosted on PID: %d\n", i, currproc->pid);
        yield();
        k_sleep(1000);
    }
    kill();
}

void proc1(void) {
    k_printf("\nStart van proc1 op PID: %d\n", currproc->pid);
    for (uint8_t i = 0; i < 4; i++){
        k_printf("currently in loop %d from proc1 hosted on PID: %d\n", i, currproc->pid);
        yield();
        k_sleep(1000);
    }
    kill();
}

void MNU_init(void){
        k_printf("\n=== Enabling MMU ===\n");
    
    uint32_t pdbr_value = (uint32_t)(uintptr_t)currproc->pdbr;
    uint32_t ppn = (pdbr_value >> 12) & 0x003FFFFF;
    uint32_t satp_value = SV32_MMU_ON | ppn;
    
    k_printf("  pdbr=%x\n", pdbr_value);
    k_printf("  ppn=%x\n", ppn);
    k_printf("  SATP= %x\n", satp_value);
        __asm__ __volatile__(
        "csrw satp, %0\n"
        "sfence.vma\n"
        :
        : "r" (satp_value)
        : "memory"
    );
    k_printf("\n===MNU is now online===\n");
}

void kernel_main(void) {
    kernel_bootstrap();
    
    k_printf("\n=== Creating test user processes ===\n");
    
    spawn_proc((uint32_t)proc0);
    spawn_proc((uint32_t)proc1);
    
    k_sp();
    k_printf("sizeof pcb_list: %d\n\n", sizeof(proclist));
    MNU_init();//start de mnu voordat de sheduler begint 
    k_printf("Round-robin cooperative scheduler is now starting:\n\n");
    yield();
    
    k_panic("Returned to kernel_main after yield!, now idling foreva jipeeeee . . .", "");
}

__attribute__((section(".text.boot")))
__attribute__((naked))
void boot(void) {
    __asm__ __volatile__(
        "add sp, zero, %[stack_top]\n"
        "jal zero, kernel_main\n"
        :
        : [stack_top] "r" (__stack_top)
        : "memory"
    );
}
