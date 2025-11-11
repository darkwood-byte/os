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

void uart_putchar(char c) {
    while ((UART_LSR & LSR_TX_IDLE) == 0);
    UART_THR = c;
}

char uart_getchar(void) {
    while ((UART_LSR & LSR_DATA_READY) == 0);
    return UART_RHR;
}

uint32_t uart_getchar_nb(char *c) {
    if (UART_LSR & LSR_DATA_READY) {
        *c = UART_RHR;
        return 1;
    }
    return 0;
}

uint32_t get_uart_input(char *uart_input_buffer, uint32_t max_len) {
   uint32_t idx = 0;
    
    while (1) {
        char c = uart_getchar();
        
        if (c == '\b' || c == 0x7F) {
            if (idx > 0) {
                idx--;
                uart_putchar('\b');
                uart_putchar(' ');
                uart_putchar('\b');
            }
            continue;
        }
        
        if (c == '\r' || c == '\n') {
            uart_putchar('\r');
            uart_putchar('\n');
            uart_input_buffer[idx] = '\0';
            return idx;
        }
        
        if (idx < max_len - 1) {
            uart_input_buffer[idx++] = c;
            uart_putchar(c);
        }
    }
}

#endif
