#ifndef SHELL_C
#define SHELL_C

#include "shell.h"

uint8_t handle_simple(char *input) {
    if (!strcmp(input, "cd")) {
        change_directory("~");
        return 1;
    }
    else if (!strcmp(input, "root")) {
        change_directory("/");
        return 1;
    }
    else if (!strcmp(input, "ls")) {
        ls_simple();
        return 1;
    }
    else if (!strcmp(input, "quit") || !strcmp(input, "exit")) {
        printk("Goodbye!\n");

        __asm__  __volatile__ ("li a7, 0x53525354\n"
                      "li a6, 0x0\n"
                      "li a0, 0x0\n"
                      "li a1, 0x0\n"
                      "ecall");
        return 1;
    }
    else if (!strcmp(input, "cory")) {
        print_cory();
        return 1;
    }
    else if (!strcmp(input, "logo")) {
        print_start();
        return 1;
    }
    else if (!strcmp(input, "mem")) {
        printk("\n------------------\nused memory: %d\nfree memory: %d\n------------------", get_used_memory(), get_free_memory());
        return 1;
    }
    else if (!strcmp(input, "help")) {
        printk("\n  ----help_menu--------------------------------------\n cd: moves you to a directory,  if none is given it returns home.\n root:moves you to the root folder\n ls: shows the contents of the current folder\n quit: closes the os\n cory: draws the mascot\n logo: draws the logo\n mem: shows memory ussage:\n touch: this makes a file with the name you give it\n mdir: makes a folder\n help: helps you find the commands =]\n  ---------------------------------------------------\n\n");
        return 1;
    }
    else
    return 0;
}

void handle_complex(char *input) {
    if (strncmp(input, "touch ", 6) == 0) {
        touch(input + 6);
        return;
    }
    else if (strncmp(input, "mkdir ", 6) == 0) {
        mkdir(input + 6);
        return;
    }
    else if (strncmp(input, "cd ", 3) == 0) {
        change_directory(input + 3);
        return;
    }
    else if (starts_with(input, "ls ")) {
        ls_complex(input + 3);
        return;
    }
    printk("--bronx: unknown command: %s\n", input);
}

void bronx(void) {
    char input[128];
    uint32_t len = get_uart_input(input, sizeof(input));
    toLowerCase(input);
    if (!handle_simple(input)) handle_complex(input);
    (void)len;
    return;
}

#endif
