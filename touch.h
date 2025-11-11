#ifndef TOUCH_H
#define TOUCH_H

#include "types.h"
#include "file.h"
#include "memory.h"
#include "printk.h"

item *makenewfile(const char *name);
void touch(char *name);
void mkdir(const char *name);

#endif
