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
    
    // Sla de huidige directory diepte op
    uint8_t depth_counter = 0;
    
    // Maak een kopie van args om te tokenizen (om de originele string niet te wijzigen)
    char path_copy[256];
    strcpy(path_copy, args);
    
    // Tokenize het pad op '/' scheidingstekens
    char *token = strtok(path_copy, "/");
    uint8_t result = 0;
    
    // Probeer naar elke directory in het pad te gaan
    while (token != NULL) {
        result = change_directory(token);
        
        if (result == 1) {
            // Directory bestaat niet
            printk("Directory '%s' does not exist\n", token);
            
            // Ga terug naar de oorspronkelijke directory
            while (depth_counter > 0) {
                change_directory("..");
                depth_counter--;
            }
            
            // Voer ls_simple uit in de oorspronkelijke directory
            ls_simple();
            return;
        } else {
            // Directory change succesvol, verhoog de teller
            depth_counter++;
        }
        
        token = strtok(NULL, "/");
    }
    
    // Als we hier zijn, zijn we succesvol naar de doel directory gegaan
    // Voer ls_simple uit in de nieuwe directory
    ls_simple();
    
    // Ga terug naar de oorspronkelijke directory
    while (depth_counter > 0) {
        change_directory("..");
        depth_counter--;
    }
}

#endif
