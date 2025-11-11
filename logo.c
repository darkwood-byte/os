#ifndef LOGO_C
#define LOGO_C

#include "logo.h"

void print_cory(void){
    printk("\n\n        /\\\n      _/./\n    ,-'    `-:..-'/\n   : o )      _  (\n   \"`-....,--; `-.\\\\\n       `'\\\n");
    return;
}

void print_cory_os(void){
     printk("\n\n  ____                    ___  ____  \n / ___|___  _ __ _   _   / _ \\/ ___| \n| |   / _ \\| '__| | | | | | | \\___ \\ \n| |__| (_) | |  | |_| | | |_| |___) |\n \\____\\___/|_|   \\__, |  \\___/|____/ \n                 |___/               \n");
    return;
}

void print_start(void){
    print_cory();
    print_cory_os();
    printk("\n\nUART initialized at 0x%x\n", UART_BASE);
}
#endif
