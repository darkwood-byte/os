#ifndef FILE_C
#define FILE_C

#include "file.h"

// Helper function to find item in current directory
item *find_item(const char *name) {
    item *current = get_current_directory();
    item *curr_item = current->child;
    
    while (curr_item != NULL) {
        if (strcmp(curr_item->name, name) == 0) {
            return curr_item;
        }
        curr_item = curr_item->next;
    }
    return NULL;
}

// Recursive helper function for get_parent_directory
static item *search_parent(item *current, item *child) {
    if (current == NULL) return NULL;
    
    // Check direct children
    item *curr_child = current->child;
    while (curr_child != NULL) {
        if (curr_child == child) {
            return current;
        }
        curr_child = curr_child->next;
    }
    
    // Recursively search subdirectories
    curr_child = current->child;
    while (curr_child != NULL) {
        if (IS_FOLDER(curr_child)) {
            item *result = search_parent(curr_child, child);
            if (result != NULL) return result;
        }
        curr_child = curr_child->next;
    }
    
    return NULL;
}

item *get_parent_directory(item *child) {
    if (child == root_directory || child == NULL) {
        return NULL;
    }
    
    return search_parent(root_directory, child);
}

item *get_current_directory(void) {
    if (current_directory == NULL) {
        // Initialize filesystem if not done yet
        root_directory = malloc(sizeof(item));
        root_directory->name = "root";
        root_directory->data = NULL;
        root_directory->next = NULL;
        root_directory->child = NULL;
        root_directory->size = 0;
        
        // Create home directory
        item *home = malloc(sizeof(item));
        home->name = "home";
        home->data = NULL;
        home->next = NULL;
        home->child = NULL;
        home->size = 0;
        
        root_directory->child = home;
        current_directory = home;
    }
    return current_directory;
}

#endif
