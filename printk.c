#ifndef PRINTK_C
#define PRINTK_C

#include "printk.h"

void putchar(char ch) {
    sbi_call(ch, 0, 0, 0, 0, 0, 0, 0x01, ch);
}

static void utoa(uint32_t num, char *buf, uint32_t base) {
    char temp[32];
    uint32_t i = 0;
    uint32_t digit;
    
    if (num == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return;
    }
    
    while (num != 0) {
        digit = num % base;
        if (digit < 10) {
            temp[i] = (char)('0' + digit);
        } else {
            temp[i] = (char)('a' + digit - 10);
        }
        i++;
        num = num / base;
    }
    
    uint32_t j = 0;
    while (i > 0) {
        i--;
        buf[j] = temp[i];
        j++;
    }
    buf[j] = '\0';
}

static void itoa(int32_t num, char *buf, uint32_t base) {
    if (num < 0 && base == 10) {
        buf[0] = '-';
        buf++;
        num = -num;
    }
    utoa((uint32_t)num, buf, base);
}

void printk(const char *fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, fmt);
    
    const char *p = fmt;
    char buf[32];
    
    while (*p != '\0') {
        if (*p != '%') {
            putchar(*p);
            p++;
            continue;
        }
        
        p++;
        
        switch (*p) {
            case 'd': {
                int32_t num = __builtin_va_arg(args, int32_t);
                itoa(num, buf, 10);
                char *s = buf;
                while (*s != '\0') {
                    putchar(*s);
                    s++;
                }
                break;
            }
            case 'u': {
                uint32_t num = __builtin_va_arg(args, uint32_t);
                utoa(num, buf, 10);
                char *s = buf;
                while (*s != '\0') {
                    putchar(*s);
                    s++;
                }
                break;
            }
            case 'x': {
                uint32_t num = __builtin_va_arg(args, uint32_t);
                utoa(num, buf, 16);
                char *s = buf;
                while (*s != '\0') {
                    putchar(*s);
                    s++;
                }
                break;
            }
            case 's': {
                char *s = __builtin_va_arg(args, char*);
                if (s == NULL) {
                    s = "(null)";
                }
                while (*s != '\0') {
                    putchar(*s);
                    s++;
                }
                break;
            }
            case 'c': {
                char c = (char)__builtin_va_arg(args, int);
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
    
    __builtin_va_end(args);
}

#endif