#include "main.h"
extern char __bss[], __bss_end[], __stack_top[];
extern char __free_ram_start[], __free_ram_end[];

void kernel_main(void) {
    memset(__bss, 0, ((size_t)__bss_end - (size_t)__bss));
    write_csr(stvec, (uint32_t)switch_trap);
    init_memory();
    
    k_printf("\n=== Kernel Boot ===\n");
    k_printf("BSS initialized\n");
    k_printf("Trap handler registered at: %p\n", (uint32_t)switch_trap);
    k_printf("Free RAM: %p - %p\n", (uint32_t)__free_ram_start, (uint32_t)__free_ram_end);
    
    // Opdracht 1: CSR test
    write_csr(sscratch, 0xDEADBEEF);
    uint32_t ss_val = read_csr(SSCRATCH);
    k_printf("CSR test: sscratch = %x\n", ss_val);
    uint32_t stvec_val = read_csr(STVEC);
    k_printf("CSR test: stvec = %p\n", stvec_val);
    
    // Opdracht 2: Kernel trap test
    k_printf("\n=== Testing Illegal Instruction Trap ===\n");
    __asm__ __volatile__("csrw scountovf, x0");
    k_printf("We hebben een kernel trap overleefd!\n");
    
    // Weekopdracht: Page frame allocator
    k_printf("\n=== Testing Page Frame Allocator ===\n");
    pframe_addr_t pfa0 = pageframalloc(3);
    k_printf("pfa0 = %p\n", pfa0);
    pframe_addr_t pfa1 = pageframalloc(1);
    k_printf("pfa1 = %p\n", pfa1);
    
    k_panic("boot-up succeeded, what we do?!", "");
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
