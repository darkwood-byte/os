#ifndef VARIADIC_H
#define VARIADIC_H

#include "types.h"

typedef struct {
    uint32_t reg_save[7]; // a1-a7 variadic arguments
    uint32_t reg_index;   // huidig register
} va_list_rv;

#define va_start_rv(ap, last)        \
    do {                                                                   \
        __asm__ volatile (                             \
            "mv %0, a1\n\t"                                 \
            "mv %1, a2\n\t"                                  \
            "mv %2, a3\n\t"                                  \
            "mv %3, a4\n\t"                                  \
            "mv %4, a5\n\t"                                  \
            "mv %5, a6\n\t"                                  \
            "mv %6, a7\n\t"                                  \
            : "=r"(ap.reg_save[0]),               \
              "=r"(ap.reg_save[1]),               \
              "=r"(ap.reg_save[2]),               \
              "=r"(ap.reg_save[3]),               \
              "=r"(ap.reg_save[4]),               \
              "=r"(ap.reg_save[5]),               \
              "=r"(ap.reg_save[6])                \
        );                                                                        \
        ap.reg_index = 0;                                   \
    } while (0)

#define va_arg_rv(ap, type) \
    ((ap.reg_index < 7) ? \
        (type)(ap.reg_save[ap.reg_index++]) : \
        (type)NULL)//als er meer als 6 ellementen gegeven wordt er een NULL meegegeven

#define va_end_rv(ap) \
    (ap.reg_index = 0)

#endif
