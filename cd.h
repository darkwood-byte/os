#ifndef CD_H
#define CD_H

#include "file.h"
#include "printk.h"

void change_directory(const char *target);
void build_full_path(void);

#endif
