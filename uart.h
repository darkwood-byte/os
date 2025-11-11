#ifndef UART_H
#define UART_H

#include "types.h"

#define UART_BASE 0x10000000
#define UART_THR  (*(volatile uint8_t *)(UART_BASE + 0))
#define UART_RHR (*(volatile uint8_t*)(UART_BASE + 0))
#define UART_LSR (*(volatile uint8_t*)(UART_BASE + 5))

#define LSR_DATA_READY (1 << 0)
#define LSR_TX_IDLE    (1 << 5)

void uart_putc(char c);

void uart_puts(const char *s) ;

 void uart_putchar(char c);

 char uart_getchar(void) ;

uint32_t  uart_getchar_nb(char *c);

uint32_t get_uart_input(char *uart_input_buffer, uint32_t max_len);

#endif
