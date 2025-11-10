#ifndef  PRINTK_C
#define PRINTK_C

#include "printk.h"

// Converteer unsigned integer naar string
 void utoa(uint32_t num, char *buf, int base) {
    uint32_t i = 0;
    
    if (num == 0) {
        buf[i++] = '0';
        buf[i] = '\0';
        return;
    }
    
    while (num != 0) {
       uint32_t rem = (uint32_t)num % (uint32_t)base;
        buf[i++] =  (uint8_t)((rem > 9) ? (rem - 10) + 'a' : rem + '0');
        num = num /(uint32_t) base;
    }
    
    buf[i] = '\0';
    
    // Reverse string
    uint32_t start = 0;
    uint32_t end = i - 1;
    while (start < end) {
        char tmp = buf[start];
        buf[start] = buf[end];
        buf[end] = tmp;
        start++;
        end--;
    }
}

// Simpele printk - gebruikt __builtin_va_list voor correcte variadic handling
typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)

void printk(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    
    const char *p;
    char buf[32];
    
    for (p = fmt; *p; p++) {
        if (*p != '%') {
            uart_putc(*p);
            continue;
        }
        
        p++;
        switch (*p) {
            case 'd': // Decimaal getal
            case 'u': // Unsigned decimaal
                utoa(va_arg(args, uint32_t), buf, 10);
                uart_puts(buf);
                break;
            case 'x': // Hexadecimaal getal
                utoa(va_arg(args, uint32_t), buf, 16);
                uart_puts(buf);
                break;
            case 's': // String
                uart_puts(va_arg(args, char*));
                break;
            case 'c': // Character
                uart_putc((char)va_arg(args, int));
                break;
            case '%': // Letterlijk %
                uart_putc('%');
                break;
            default:
                uart_putc('%');
                uart_putc(*p);
                break;
        }
    }
    
    va_end(args);
}

#endif
