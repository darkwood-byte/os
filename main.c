// === DATASTRUCTUREN ===
#include "main.h"
#define MAXPROCS 4

extern char __bss[], __bss_end[], __stack_top[];
extern char __free_ram_start[], __free_ram_end[];

typedef enum {
    NOPROC,
    READY,
    RUNNING,
    BLOCKED
} procstate;

typedef struct {
    uint32_t pid;
    procstate pstate;
    uint32_t psp;
    uint8_t pstack[1024];
} pcb;

pcb proclist[MAXPROCS];
pcb *currproc = NULL;
pcb *idleproc = NULL;

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

// === SPAWN_PROC ===
pcb *spawn_proc(uint32_t entrypoint) {
    pcb *p = NULL;
    
    // Zoek vrije plek
    for(uint32_t i = 0; i < MAXPROCS; i++) {
        if(proclist[i].pstate == NOPROC) {
            p = &proclist[i];
            p->pid = i;
            break;
        }
    }
    
    if(!p) {
        k_panic("No free PCB", "");
        return NULL;
    }
    
    memset(p->pstack, 0, sizeof(p->pstack));
    p->pstate = READY;
    
    // Bereken stack top
    uintptr_t stack_top = (uintptr_t)&p->pstack[0] + sizeof(p->pstack);
    uint32_t *sp = (uint32_t*)stack_top;
    
    // Align op 4 bytes
    if (((uintptr_t)sp & 0x3) != 0) {
        sp = (uint32_t*)(((uintptr_t)sp) & ~(uint32_t)0x3);
    }
    
    // Push 12 nullen voor s11-s0
    for (int i = 0; i < 12; i++) {
        sp--;
        *sp = 0;
    }
    
    // Push entrypoint als ra
    sp--;
    *sp = (uint32_t)entrypoint;
    
    p->psp = (uint32_t)(uintptr_t)sp;
    
    return p;
}

// === SWITCH_PROC volgens document ===
__attribute__((naked))
void switch_proc(uint32_t *current_psp, uint32_t *next_psp) {
    __asm__ __volatile__(
        // Stap 1: Save registers op huidige stack
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
        
        // Stap 2 & 3: Switch stack pointer
        // *current_psp = sp
        "sw sp, 0(a0)\n"
        // sp = *next_psp
        "lw sp, 0(a1)\n"
        
        // Stap 4: Restore registers van nieuwe stack
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

// === YIELD functie volgens document ===
void yield(void) {
    // Save old currproc voor switch_proc
    pcb *oldproc = currproc;
    
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
            break;
        }
    }
    
    // Als niks gevonden, check of huidig proces zelf READY is
    if (!nextproc && currproc && currproc->pstate == READY) {
        nextproc = currproc;
    }
    
    // Als GEEN ENKEL proces READY is, val terug naar idle (PID 0)
    if (!nextproc) {
        nextproc = idleproc;
    }
    
    // Als hetzelfde proces, gewoon return (geen switch nodig)
    if (nextproc == oldproc) {
        return;
    }
    
    // Update currproc
    currproc = nextproc;
    currproc->pstate = RUNNING;
    
    // Context switch
    switch_proc(&oldproc->psp, &nextproc->psp);
}

void k_sleep(uint32_t ms){
    for (uint32_t i = 0; 200000 * ms > i; i++){
        __asm__  __volatile__ ("nop");
    }
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

#define start(id)do{\
    proclist[id].pstate = start;\
    k_printf("started proces id: %d\n", id);\
}while(0);

#define block()do{\
    currproc->pstate = NOPROC;\
    k_printf("blocked proces id:  %d\n");\
    yield();\
}while(0);

#define kill()do{\
    currproc->pstate = NOPROC;\
    k_printf("killed proces id:  %d\n");\
    yield();\
}while(0);

// === TEST PROCESSEN ===
void proc0(void) {
    k_printf("\nStart van PID %d\n", currproc->pid);
     while(1)  {
        k_printf("proc0 had pid:  %d\n", currproc->pid);
        yield();
        k_sleep(1000);
    }
    kill();
}

void proc1(void) {
    k_printf("\nStart van PID %d\n", currproc->pid);
     k_printf("proc1 had pid: %d\n", currproc->pid);
     yield();
    k_sleep(1000);
    yield();
     k_sleep(1000);
    yield();
     k_sleep(1000);
    yield();
     k_sleep(1000);
     k_printf("stoping1\n");
    kill();
}

// === KERNEL_MAIN ===
void kernel_main(void) {
    kernel_boot();
    
    // Spawn idle proces (PID 0) met NULL entrypoint
    idleproc = spawn_proc((uint32_t)NULL);
    currproc = idleproc;
    
    // Spawn user processen
    spawn_proc((uint32_t)proc0);
    spawn_proc((uint32_t)proc1);
    
    k_sp();
    k_printf("sizeof pcb_list: %d\n\n", sizeof(proclist));
    
    yield();
    
    k_panic("\nboot-up succeeded, now back in PID 0 (idlin')...\n", "");;
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