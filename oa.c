#include "oa.h"

void utoa(uint32_t num, char *buf, uint32_t base) {
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

 void itoa(int32_t num, char *buf, uint32_t base) {
    if (num < 0 && base == 10) {
        buf[0] = '-';
        utoa((uint32_t)(-num), buf + 1, base);
    } else {
        utoa((uint32_t)num, buf, base);
    }
}
