#ifndef CD_C
#define CD_C

#include "cd.h"

void build_full_path(void) {
    if (current_directory == root_directory) {
        path = "/";
        return;
    }
    
    int depth = 0;
    item *temp = current_directory;
    while (temp != NULL && temp != root_directory) {
        depth++;
        temp = get_parent_directory(temp);
    }
    
    char *path_components[depth];
    temp = current_directory;
    for (int i = depth - 1; i >= 0; i--) {
        path_components[i] = temp->name;
        temp = get_parent_directory(temp);
    }
    
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

uint8_t change_directory(const char *target) {
    if (target == NULL || strlen(target) == 0 || strcmp(target, "~") == 0) {
        item *home = root_directory->child;
        while (home != NULL && strcmp(home->name, "home") != 0) {
            home = home->next;
        }
        if (home != NULL) {
            current_directory = home;
        }
        build_full_path();
        return 0;
    } else if (strcmp(target, "/") == 0) {
        current_directory = root_directory;
        build_full_path();
        return 0;
    } else if (strcmp(target, "..") == 0) {
        item *parent = get_parent_directory(current_directory);
        if (parent != NULL) {
            current_directory = parent;
        }
        build_full_path();
        return 0;
    }
    
    uint8_t depth_counter = 0;
    char path_copy[256];
    strcpy(path_copy, target);
    
    char *token = strtok(path_copy, "/");
    
    while (token != NULL) {
        if (strcmp(token, "..") == 0) {
            item *parent = get_parent_directory(current_directory);
            if (parent != NULL) {
                current_directory = parent;
                depth_counter++;
            } else {
                printk("cd: Cannot go above root directory\n");
                
                while (depth_counter > 0) {
                    item *parent_dir = get_parent_directory(current_directory);
                    if (parent_dir != NULL) {
                        current_directory = parent_dir;
                    }
                    depth_counter--;
                }
                
                build_full_path();
                return 1;
            }
        } else if (strcmp(token, ".") == 0) {
            depth_counter++;
        } else {
            item *target_dir = find_item(token);
            if (target_dir != NULL && IS_FOLDER(target_dir)) {
                current_directory = target_dir;
                depth_counter++;
            } else {
                printk("cd: %s: No such directory\n", token);
                
                while (depth_counter > 0) {
                    item *parent_dir = get_parent_directory(current_directory);
                    if (parent_dir != NULL) {
                        current_directory = parent_dir;
                    }
                    depth_counter--;
                }
                
                build_full_path();
                return 1;
            }
        }
        
        token = strtok(NULL, "/");
    }
    
    build_full_path();
    return 0;
}

#endif
