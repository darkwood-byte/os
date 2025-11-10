#ifndef UART_H
#define UART_H

#include "types.h"

#define UART_BASE 0x10000000
#define UART_THR  (*(volatile uint8_t *)(UART_BASE + 0))

void uart_putc(char c);

void uart_puts(const char *s) ;

#endif
