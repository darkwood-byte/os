#include "main.h"

extern char __bss[], __bss_end[], __stack_top[];
extern char __free_ram_start[], __free_ram_end[];
extern char __kernel_base[];

void kernel_bootstrap(void){
    k_printf("\n=== Kernel Boot ===\n");
    
    // Initialize BSS
    memset(__bss, 0, ((size_t)__bss_end - (size_t)__bss));
    k_printf("BSS initialized\n");
    
    // Register trap handler
    write_csr(stvec, (uint32_t)switch_trap);
    k_printf("Trap handler registered at: %p\n", (uint32_t)switch_trap);

    // Initialize memory allocator
    init_memory();
    k_printf("Memory initialized\n");
    k_printf("Free RAM: %p - %p\n", (uint32_t)__free_ram_start, (uint32_t)__free_ram_end);
    
    // Create kernel idle process (PID 0)
    k_printf("\n=== Creating Kernel Idle Process ===\n");
    idleproc = spawn_proc((uint32_t)NULL, (uint32_t)NULL);
    if (!idleproc) {
        k_panic("Failed to create idle process\n", "");
    }
    
    currproc = idleproc;
    k_printf("Idle process created (PID %d)\n", idleproc->pid);
    
    // *** CRITICAL: Activate MMU for kernel ***
    k_printf("\n=== Activating MMU ===\n");
    k_printf("Switching to virtual memory mode...\n");
    k_printf("Kernel window: VA 0x%x maps to PA 0x%x\n", KERNEL_VIRT_BASE, KERNEL_PHYS_BASE);
    
    Flush_TLB(idleproc);
    
    // Mark that MMU is now active
    set_mmu_active();
    
    k_printf("MMU ACTIVE! All memory accesses now virtualized.\n");
    k_printf("Kernel can access physical memory via kernel window.\n");
    
    k_printf("\n=== Kernel Boot Complete ===\n\n");
}


// Binary images linked by linker
extern char _binary_init_bin_start[];
extern char _binary_init_bin_size[];

extern char _binary_bronx_bin_start[];
extern char _binary_bronx_bin_size[];

extern char _binary_test_bin_start[];
extern char _binary_test_bin_size[];

void kernel_main(void) {
    kernel_bootstrap();
    
    k_printf("=== Starting User Programs ===\n");
    
    // Register applications
    init_app("init", _binary_init_bin_start, _binary_init_bin_size);
    init_app("brox", _binary_bronx_bin_start, _binary_bronx_bin_size);
    init_app("test", _binary_test_bin_start, _binary_test_bin_size);
    k_printf("Applications registered\n");
    
    // Start init program
    k_printf("\nStarting init program...\n");
    uint32_t init_pid = start_app(0);  // Assuming init is at index 0
    k_printf("Init started with PID %d\n", init_pid);
    
    k_printf("\n=== Yielding to scheduler ===\n");
    yield();
    
    // Should never reach here (idle loop)
    k_panic("Returned to kernel_main (PID 0 idling)...", "");
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
