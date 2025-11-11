#ifndef SHELL_H
#define SHELL_H

#include "types.h"
#include "file.h"
#include "printk.h"
#include "logo.h"
#include "memory.h"
#include "uart.h"
#include "cd.h"
#include "ls.h"
#include "touch.h"

uint8_t handle_simple(char *input);
void handle_complex(char *input);
void bronx(void);

#endif
