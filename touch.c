#ifndef TOUCH_C
#define TOUCH_C

#include "touch.h"


item *makenewfile(const char *name) {
    if (name == NULL || name[0] == '\0') {
        printk("makenewfile: missing filename\n");
        return NULL;
    }

    if (find_item(name) != NULL) {
        printk("makenewfile: '%s' already exists\n", name);
        return NULL;
    }

    item *new_file = malloc(sizeof(item));
    if (new_file == NULL) {
        printk("makenewfile: insufficient memory\n");
        return NULL;
    }

    new_file->name = malloc(strlen(name) + 1);
    if (new_file->name == NULL) {
        free(new_file);
        printk("makenewfile: insufficient memory\n");
        return NULL;
    }
    strcpy(new_file->name, name);

    file *file_data = malloc(sizeof(file));
    if (file_data == NULL) {
        free(new_file->name);
        free(new_file);
        printk("makenewfile: insufficient memory\n");
        return NULL;
    }

    file_data->size = 0;
    file_data->data = NULL;
    
    new_file->data = file_data;
    new_file->size = 0;
    new_file->next = NULL;
    new_file->child = NULL;

    item *current = get_current_directory();
    if (current->child == NULL) {
        current->child = new_file;
    } else {
        item *last = current->child;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = new_file;
    }

    return new_file;
}

void mkdir(const char *name) {
    if (name == NULL || name[0] == '\0') {
        printk("mkdir: missing directory name\n");
        return;
    }

    if (find_item(name) != NULL) {
        printk("mkdir: '%s' already exists\n", name);
        return;
    }

    item *new_dir = malloc(sizeof(item));
    if (new_dir == NULL) {
        printk("mkdir: insufficient memory\n");
        return;
    }

    new_dir->name = malloc(strlen(name) + 1);
    if (new_dir->name == NULL) {
        free(new_dir);
        printk("mkdir: insufficient memory\n");
        return;
    }
    strcpy(new_dir->name, name);

    new_dir->data = NULL;
    new_dir->size = 0;
    new_dir->next = NULL;
    new_dir->child = NULL;

    item *current = get_current_directory();
    if (current->child == NULL) {
        current->child = new_dir;
    } else {
        item *last = current->child;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = new_dir;
    }

    printk("Created directory '%s'\n", name);
}

void touch(char *name) {
    item *new_file = makenewfile(name);
    if (new_file != NULL) {
        printk("Created file '%s'\n", name);
    }
}

#endif
