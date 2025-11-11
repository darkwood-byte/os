#include "main.h"

#define IS_FOLDER(item) ((item)->data == NULL)


extern char __bss[], __bss_end[], __stack_top[];

typedef struct {
    uint32_t size;
    uint8_t *data;
} file;

typedef struct item item;
struct item {
    char *name;
    file *data;
    item *next;
    item *child;
    uint32_t size;
};

void bronx(void) {
    char input[128];
    uint32_t len = get_uart_input(input, sizeof(input));
    
    printk("Je typte: '%s'\n", input);
    printk("Lengte: %d chars\n", len);
    return;
}



void kernel_main(void) {
    memset(__bss, 0, ((size_t)__bss_end - (size_t)__bss));
    
   print_start();
    
    char *path = "/home";
    while (1) {
        printk("\n~%s: ",path);
        bronx();
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
