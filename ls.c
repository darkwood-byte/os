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
    if(change_directory(args))return;
    ls_simple();
    uint32_t depth_counter = 1;
    uint8_t flag = 0;
   for (uint32_t i = 0; args[i] !='\0'; i++ ){
        flag = 0;
        if (args[i] == '/'){
            depth_counter++;
            flag = 1;
        }
   }
   if (flag)depth_counter--;
    while (depth_counter > 0) {
        change_directory("..");
        depth_counter--;
    }
}

#endif
