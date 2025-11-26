#ifndef PRINTK_H
#define PRINTK_H

#include "types.h"
#include "SBI.h"
#include "variadic.h"
#include "oa.h"

void k_printf(const char *fmt, ...);

#endif
