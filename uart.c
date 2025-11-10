#ifndef UART_C
#define UART_C

#include "uart.h"

void uart_putc(char c) {
    UART_THR = c;
}

void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

#endif
