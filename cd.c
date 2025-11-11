#ifndef CD_C
#define CD_C

#include "cd.h"

void build_full_path(void) {
    if (current_directory == root_directory) {
        path = "/";
        return;
    }
    
    // Count path components
    int depth = 0;
    item *temp = current_directory;
    while (temp != NULL && temp != root_directory) {
        depth++;
        temp = get_parent_directory(temp);
    }
    
    // Build path backwards
    char *path_components[depth];
    temp = current_directory;
    for (int i = depth - 1; i >= 0; i--) {
        path_components[i] = temp->name;
        temp = get_parent_directory(temp);
    }
    
    // Allocate and build path
    static char path_buffer[256];
    char *ptr = path_buffer;
    *ptr++ = '/';
    
    for (int i = 0; i < depth; i++) {
        char *name = path_components[i];
        while (*name) {
            *ptr++ = *name++;
        }
        if (i < depth - 1) {
            *ptr++ = '/';
        }
    }
    *ptr = '\0';
    
    path = path_buffer;
}

void change_directory(const char *target) {
    if (target == NULL || strlen(target) == 0 || strcmp(target, "~") == 0) {
        // cd to home
        item *home = root_directory->child;
        while (home != NULL && strcmp(home->name, "home") != 0) {
            home = home->next;
        }
        if (home != NULL) {
            current_directory = home;
        }
    } else if (strcmp(target, "/") == 0) {
        // cd to root
        current_directory = root_directory;
    } else if (strcmp(target, "..") == 0) {
        // cd to parent
        item *parent = get_parent_directory(current_directory);
        if (parent != NULL) {
            current_directory = parent;
        }
    } else {
        // cd to subdirectory
        item *target_dir = find_item(target);
        if (target_dir != NULL && IS_FOLDER(target_dir)) {
            current_directory = target_dir;
        } else {
            printk("cd: %s: No such directory\n", target);
            return;
        }
    }
    
    build_full_path();
}

#endif
