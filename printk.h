#ifndef  PRINTK_H
#define PRINTK_H

#include "types.h"
#include "uart.h"

// Converteer unsigned integer naar string
void utoa(uint32_t num, char *buf, int base);

// Simpele printk - gebruikt __builtin_va_list voor correcte variadic handling
typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)

void printk(const char *fmt, ...) ;

#endif
