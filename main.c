#include "main.h"
#include "cd.h"

#define HEAP_START   0x80400000
#define HEAP_END     0x87E00000
#define HEAP_SIZE    (HEAP_END - HEAP_START)

extern char __bss[], __bss_end[], __stack_top[];

// Global filesystem variables
item *root_directory = NULL;
item *current_directory = NULL;
char *path = "/home";

void kernel_main(void) {
    memset(__bss, 0, ((size_t)__bss_end - (size_t)__bss));
    
    memory_init((void*)HEAP_START, HEAP_SIZE);

    print_start();
    
    current_directory = get_current_directory();
    build_full_path();

    while (1) {
        printk("\n~%s: ", path);
        bronx();//the cory kernel shell
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
