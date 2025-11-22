#include "main.h"

extern char __bss[], __bss_end[], __stack_top[];



void kernel_main(void) {
    k_printf("\nBeste %s\n", "Koeien!");
    k_printf("Yes or no : %c\n", 'y');
    k_printf("39 + 3 = %d\n7 - 15 = %d\n", (39 + 3), (7 - 15));
    k_printf("Doole koe is    : %x\n", 0xDEADBEEF);
    k_printf("Doole koe is ook: %x\n", (unsigned int)3735928559);
    k_printf("Adres van BSS   : %p\n", (unsigned int)__bss);
    k_panic("test %d", 3);
    while(1);
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
