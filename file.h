#ifndef FILE_H
#define FILE_H

#include "types.h"
#include "memory.h"
#include "printk.h"

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

// String utilities
uint32_t strlen(const char *str);
int strncmp(const char *s1, const char *s2, uint32_t n);
int starts_with(const char *str, const char *prefix);
uint8_t strcmp(const char *s1, const char *s2);
void strcpy(char *dest, const char *src);
void toLowerCase(char *str);

// Directory operations
item *get_current_directory(void);
item *find_item(const char *name);
item *get_parent_directory(item *child);

#endif
