#ifndef FILE_H
#define FILE_H

#include "types.h"
#include "memory.h"
#include "printk.h"
#include "string.h"

#define IS_FOLDER(item) ((item)->data == NULL)

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

// Filesystem globals
extern item *root_directory;
extern item *current_directory;
extern char *path;



// Directory operations
item *get_current_directory(void);
item *find_item(const char *name);
item *get_parent_directory(item *child);

#endif
