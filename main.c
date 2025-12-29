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


//init 
extern char _binary_init_bin_start[];
extern char _binary_init_bin_size[];

//besh
extern char _binary_bronx_bin_start[];
extern char _binary_bronx_bin_size[];

extern char _binary_test_bin_start[];
extern char _binary_test_bin_size[];

void kernel_main(void) {
    kernel_bootstrap();
    start_app(init_app("init",_binary_init_bin_start,_binary_init_bin_size));
    init_app("brox", _binary_bronx_bin_start, _binary_bronx_bin_size);
    init_app("test", _binary_test_bin_start, _binary_test_bin_size);

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
