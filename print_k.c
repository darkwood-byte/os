#ifndef PRINTK_C
#define PRINTK_C

#include "printk.h"

void putchar(char ch) {
    sbi_call(ch, 0, 0, 0, 0, 0, 0, 0x01, ch);
}

static void utoa(uint32_t num, char *buf, uint32_t base) {
    char temp[32];
    uint32_t i = 0, digit;

    if (num == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }

    while (num != 0) {
        digit = num % base;
        temp[i++] = (char)((digit < 10)
            ? ('0' + digit)
            : ('a' + digit - 10));
        num /= base;
    }

    for (uint32_t j = 0; j < i; j++)
        buf[j] = temp[i - j - 1];

    buf[i] = '\0';
}

static void itoa(int32_t num, char *buf, uint32_t base) {
    if (num < 0 && base == 10) {
        buf[0] = '-';
        utoa((uint32_t)(-num), buf + 1, base);
    } else {
        utoa((uint32_t)num, buf, base);
    }
}

void k_printf(const char *fmt, ...) {
    va_list_rv args;
    va_start_rv(args, fmt);

    char buf[32];
    const char *p = fmt;

    while (*p) {
        if (*p != '%') {
            putchar(*p++);
            continue;
        }

        p++;

        switch (*p) {
        case 'd': {
            int32_t val = va_arg_rv(args, int32_t);
            itoa(val, buf, 10);
            for (char *s = buf; *s; s++)
                putchar(*s);
            break;
        }
        case 'u': {
            uint32_t val = va_arg_rv(args, uint32_t);
            utoa(val, buf, 10);
            for (char *s = buf; *s; s++)
                putchar(*s);
            break;
        }

        case 'x': {
            uint32_t val = va_arg_rv(args, uint32_t);
            putchar('0');
            putchar('x');
            utoa(val, buf, 16);
            for (char *s = buf; *s; s++)
                putchar(*s);
            break;
        }

        case 'p': {
            putchar('0');
            putchar('x');
            uint32_t val = va_arg_rv(args, uint32_t);
            utoa(val, buf, 16);
            for (char *s = buf; *s; s++)
                putchar(*s);
            break;
        }

        case 's': {
            char *str = va_arg_rv(args, char*);
            if (!str) str = "(null)";
            for (; *str; str++)
                putchar(*str);
            break;
        }

        case 'c': {
            char c = (char)va_arg_rv(args, int);
            putchar(c);
            break;
        }

        case '%': {
            putchar('%');
            break;
        }

        default: {
            putchar('%');
            putchar(*p);
            break;
        }
        }

        p++;
    }

    va_end_rv(args);
}

#endif
