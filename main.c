#include "main.h"

extern char __bss[], __bss_end[], __stack_top[];

void kernel_main(void) {
    memset(__bss, 0, ((size_t)__bss_end - (size_t)__bss));
    
    printk("Hello from RISC-V kernel!\n");
    
    uint32_t i = 0;
    while(1) {
        printk("Uptime is %d seconds.\n", i);
        i++;
        sleepk_ms(1000);
    }
}

__attribute__((section(".text.boot")))
__attribute__((naked))
void boot(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n"
        "j kernel_main\n"
        :
        : [stack_top] "r" (__stack_top)
    );
}
