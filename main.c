#include "main.h"

extern char __bss[], __bss_end[], __stack_top[];
extern char __free_ram_start[], __free_ram_end[];
extern char __kernel_base[];

void kernel_boot(void){
    k_printf("\nBoot done. . .\n");
    memset(__bss, 0, ((size_t)__bss_end - (size_t)__bss));
    write_csr(stvec, (uint32_t)switch_trap);
    init_memory();
    
    k_printf("\n=== Kernel Boot ===\n");
    k_printf("BSS initialized\n");
    k_printf("Trap handler registered at: %p\n", (uint32_t)switch_trap);
    k_printf("Free RAM: %p - %p\n", (uint32_t)__free_ram_start, (uint32_t)__free_ram_end);
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

void voorbeeld() {
    uint32_t addr = 0x80200000;
    
    // Of met macro:
    if (IS_PAGE_ALIGNED(addr)) {
        k_printf("yes\n");
        // Adres is page-aligned
    }
    else k_printf("no\n");

     addr = 0x80200001;
    
    // Of met macro:
 
    
    // PTE maken:
    uint32_t ppn = 0x80200;  // Voorbeeld PPN
    uint32_t pte = (ppn << 10) | PTE_FLG_V | PTE_FLG_R | PTE_FLG_W | PTE_FLG_X;

    // Maak een pageframe voor Page Directory
    uint32_t *pdbr = (uint32_t *)pageframalloc(1);

    // Map kernel geheugen
    for (uint32_t pfa = (uint32_t)__kernel_base, vpa = 0; pfa < (uint32_t)__free_ram_start; pfa += PAGEFRAMESIZE, vpa += PAGEFRAMESIZE) {
        add_ptbl_entry(pdbr, vpa, pfa, PTE_FLG_R | PTE_FLG_W | PTE_FLG_X);
        k_printf("add_ptbl: VPA=0x%x PFA=0x%x (vpa aligned: %s, pfa aligned: %s)\n",vpa, pfa, IS_PAGE_ALIGNED(vpa) ? "yes" : "no", IS_PAGE_ALIGNED(pfa) ? "yes" : "no");
    }
    // Wijs toe aan PCB
    spawn_proc((uint32_t)proc0)->pdbr = pdbr;
}

void kernel_main(void) {
    kernel_boot();//functie zodat ik niet perongeluk wat sloop
    
    // Spawn idle proces (PID 0) met NULL entrypoint om terueg te keren naar kernel
    idleproc = spawn_proc((uint32_t)NULL);
    currproc = idleproc;
    
    voorbeeld();
    // Spawn user de sander user processen
    
    spawn_proc((uint32_t)proc1);
    k_sp();
    k_printf("sizeof pcb_list: %d\n\n", sizeof(proclist));
    
    yield();
    
    k_panic("\nboot-up succeeded, now back in PID 0 (idlin forava')...\n", "");;
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