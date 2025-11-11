#ifndef LS_C
#define LS_C

#include "ls.h"

void ls_simple(void) {
    item *current = get_current_directory();
    item *curr_item = current->child;
    
    if (curr_item == NULL) {
        printk("Directory is empty\n");
        return;
    }
    
    while (curr_item != NULL) {
        if (IS_FOLDER(curr_item)) {
            printk("[%s] ", curr_item->name);
        } else {
            printk("%s ", curr_item->name);
        }
        curr_item = curr_item->next;
    }
    printk("\n");
}

void ls_complex(char *args) {
    if (args == NULL || args[0] == '\0') {
        ls_simple();
        return;
    }
    
    // Parse options
    int show_size = 0;
    int show_type = 0;
    char *arg = args;
    
    while (*arg) {
        if (*arg == '-') {
            arg++;
            while (*arg) {
                if (*arg == 'l') show_size = 1;
                if (*arg == 'f') show_type = 1;
                arg++;
            }
        } else {
            break;
        }
        if (*arg == ' ') arg++;
    }
    
    // List directory
    item *current = get_current_directory();
    item *curr_item = current->child;
    
    if (curr_item == NULL) {
        printk("Directory is empty\n");
        return;
    }
    
    while (curr_item != NULL) {
        if (show_size || show_type) {
            if (IS_FOLDER(curr_item)) {
                printk("[%s]", curr_item->name);
                if (show_type) printk("/");
                if (show_size) printk(" %dB", curr_item->size);
            } else {
                printk("%s", curr_item->name);
                if (show_type) printk("*");
                if (show_size) printk(" %dB", curr_item->size);
            }
            printk("\n");
        } else {
            if (IS_FOLDER(curr_item)) {
                printk("[%s] ", curr_item->name);
            } else {
                printk("%s ", curr_item->name);
            }
        }
        curr_item = curr_item->next;
    }
    
    if (!show_size && !show_type) {
        printk("\n");
    }
}

#endif
