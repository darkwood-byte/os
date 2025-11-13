#ifndef CD_H
#define CD_H

#include "file.h"
#include "printk.h"

uint8_t change_directory(const char *target);
void build_full_path(void);

#endif
