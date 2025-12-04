#include "main.h"
extern char __bss[], __bss_end[], __stack_top[];
extern char __free_ram_start[], __free_ram_end[];

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

#define MAXPROCS 4

typedef enum {
NOPROC,
READY,
RUNNING,
BLOCKED
} procstate;

typedef struct {
    uint32_t pid; // Process ID
    procstate pstate; // Process state
    uint32_t psp; // Process stack pointer, wijst ergens in 'pstack'
    uint8_t pstack[1024];// Process stack: 1 KiB per proces
} pcb; // Process Control Block

pcb proclist[MAXPROCS];

pcb *find_free_pcb(void){
    for(uint32_t i = 0; i < MAXPROCS; i++){
        if(proclist[i].pstate == NOPROC){
            proclist[i].pstate = READY;
            return &proclist[i];
        }
    }
    k_panic("no free PCB found due to max proces count, proces id: %d", MAXPROCS);
}

pcb *spawn_proc(uint32_t entrypoint){
    pcb *p = find_free_pcb();
    if(!p) return NULL;

    /* 1) Zorg dat stack eerst nul is (niet strikt noodzakelijk, maar helpt bij debugging) */
    memset(p->pstack, 0, sizeof(p->pstack));

    /* 2) p->pid is al gezet door find_free_pcb; zet pstate op READY */
    p->pstate = READY;

    /* 3) Bepaal top van pstack: we willen psp wijzen naar het laatst gepushte 32-bit woord.
       p->pstack is een byte-array; we gebruiken een uint32_t-pointer voor pushes. */
    uintptr_t stack_base = (uintptr_t)&p->pstack[0];
    uintptr_t stack_top  = stack_base + sizeof(p->pstack); /* adres nét boven laatste byte */

    /* Zorg alignment: sp moet 4-byte aligned (althans voor 32-bit pushes) */
    uint32_t *sp = (uint32_t*) (stack_top);
    /* Als stack_top niet 4-aligned is, corrigeer */
    if (((uintptr_t)sp & 0x3) != 0) {
        sp = (uint32_t*)(((uintptr_t)sp) & ~(uint32_t) 0x3);
    }

    for (int i = 0; i < 12; i++) {
        sp--;      /* stack groeit naar lagere adressen */
        *sp = 0;   /* init alle register plaatsen met 0 */
    }

    sp--;
    *sp = (uint32_t)entrypoint;

    p->psp = (uint32_t)(uintptr_t)sp;

    return p;
}

void proces_handler(void){
    return;
}

void k_sp(void){
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

void kernel_main(void) {
    kernel_boot();
    spawn_proc(0);
    spawn_proc(0);
    spawn_proc(0);
    k_sp();
    k_printf("sizeof pcb_list: %d\n\n", sizeof(proclist));
    proces_handler();

    k_panic("\nboot-up succeeded, what we do?!\n", "");
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
