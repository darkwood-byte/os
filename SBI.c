#ifndef SBI_C
#define SBI_C

#include "SBI.h"

sbiret sbi_call(uint32_t arg0, uint32_t arg1, uint32_t arg2,
    uint32_t arg3, uint32_t arg4, uint32_t arg5,
    uint32_t fid, uint32_t eid, char ch)

{
    sbiret ret;
    ret.error = 0;
    ret.value = 0;

    register uintptr_t a0 __asm__("a0") = (uintptr_t)(arg0);
    register uintptr_t a1 __asm__("a1") = (uintptr_t)(arg1);
    register uintptr_t a2 __asm__("a2") = (uintptr_t)(arg2);
    register uintptr_t a3 __asm__("a3") = (uintptr_t)(arg3);
    register uintptr_t a4 __asm__("a4") = (uintptr_t)(arg4);
    register uintptr_t a5 __asm__("a5") = (uintptr_t)(arg5);
    register uintptr_t a6 __asm__("a6") = (uintptr_t)(fid);
    register uintptr_t a7 __asm__("a7") = (uintptr_t)(eid);


    __asm__ __volatile__(
        "ecall"
        : "+r"(a0), "+r"(a1) 
        : "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7) 
        : "memory"
    );

    __asm__ __volatile__(
        "li a7, 1\n"
        "ecall"
        :
        : "r"((uint32_t)ch)
        : "a7", "memory"
    );

    return ret;
}

void k_putchar(char ch) {
    sbi_call(ch, 0, 0, 0, 0, 0, 0, 0x01, ch);
}

char k_readchar(void)
{
    int ch;

    __asm__ __volatile__(
        "li a7, 2\n"      // SBI: console_getchar
        "ecall\n"
        "mv %0, a0\n"     
        : "=r"(ch)
        :
        : "a0", "a7", "memory"
    );
    
    // SBI fail
    if (ch < 0)
        return 0;

    if (ch == 4)
    {
        k_printf("\n------------------------\nGoodbye =]\n\n");
        __asm__ __volatile__(
            "li a7, 8\n"  // SBI shutdown
            "ecall\n"
            :
            :
            : "a7", "memory"
        );
        k_panic("\nSBI shutdown failed, check hardware....\n", "");
    }
    return (char)ch;
}

#endif
