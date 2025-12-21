#ifndef PRINTK_C
#define PRINTK_C

#include "print_k.h"

void k_printf(const char *fmt, ...) {
    va_list_rv args;
    va_start_rv(args, fmt);

    char buf[32];
    const char *p = fmt;

    while (*p) {
        if (*p != '%') {
            k_putchar(*p++);
            continue;
        }

        p++;

        switch (*p) {
        case 'd': {
            int32_t val = va_arg_rv(args, int32_t);
            itoa(val, buf, 10);
            for (char *s = buf; *s; s++)
                k_putchar(*s);
            break;
        }
        case 'u': {
            uint32_t val = va_arg_rv(args, uint32_t);
            utoa(val, buf, 10);
            for (char *s = buf; *s; s++)
                k_putchar(*s);
            break;
        }

        case 'x': {
            uint32_t val = va_arg_rv(args, uint32_t);
            k_putchar('0');
            k_putchar('x');
            utoa(val, buf, 16);
            for (char *s = buf; *s; s++)
                k_putchar(*s);
            break;
        }

        case 'p': {
            k_putchar('0');
            k_putchar('x');
            uint32_t val = va_arg_rv(args, uint32_t);
            utoa(val, buf, 16);
            for (char *s = buf; *s; s++)
                k_putchar(*s);
            break;
        }

        case 's': {
            char *str = va_arg_rv(args, char*);
            if (!str) str = "(null)";
            for (; *str; str++)
                k_putchar(*str);
            break;
        }

        case 'c': {
            char c = (char)va_arg_rv(args, int);
            k_putchar(c);
            break;
        }

        case '%': {
            k_putchar('%');
            break;
        }

        default: {
            k_putchar('%');
            k_putchar(*p);
            break;
        }
        }

        p++;
    }

    va_end_rv(args);
}

#endif
