#ifndef PANIC_K
#define PANIC_K

#define k_panic(fmt, ...) do { \
    k_printf("Kernel panic: %s:%d: " fmt "\n", \
             __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__); \
    while (1); \
} while (0)

#endif
