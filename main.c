

#include "printk.h"

#define NULL ((void*)0)

extern char __bss[], __bss_end[], __stack_top[];

void kernel_main(void) {
    int i = 1;
    printk("hello world %d", i);
    while(1);
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
