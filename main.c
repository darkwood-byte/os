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
    idleproc = spawn_proc((uint32_t)NULL, (uint32_t)NULL);//kernel proc
    currproc = idleproc;
    
    k_printf("\nKernel Boot done. . .\n");
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



// In je kernel.c, gebruik deze symbolen:
extern char _binary_besh_bin_start[];
extern char _binary_besh_bin_size[];

#define MAX_APPS 10

typedef struct
{
    char name[12];
    uint32_t start;
    uint32_t size;
}app;

app app_list[MAX_APPS];

//voegt een app toe aan het telfoon boekje van de kernel
uint32_t init_app(char name[12], char start[], char size[]){
    app new_app;
    for (int i = 0; i < 11 && name[i] != '\0'; i++) {
        new_app.name[i] = name[i];
    }
    new_app.name[11] = '\0';
    new_app.start = (uint32_t)start;
    new_app.size = (uint32_t)size;

    for(uint32_t i = 0; i < MAX_APPS; i++){
        if (app_list[i].size == 0){app_list[i] = new_app; return i;}
    }
    k_panic("\nno free app slots found for %s\n", name);
}

//start een app
void start_app(uint32_t app_id){
    if (app_id > MAX_APPS || app_list[app_id].size == 0)k_panic("tried to start a non vaild app_id: %d\n", app_id);
    spawn_proc(app_list[app_id].start, app_list[app_id].size);
}

void kernel_main(void) {
    kernel_bootstrap();
    
    start_app( init_app("besh", _binary_besh_bin_start, _binary_besh_bin_size));
    yield();
    
    k_panic("now in PID 0 (idlin') ...", "");
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
