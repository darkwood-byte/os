#ifndef SBI_C
#define SBI_C

#include "SBI.h"

sbiret sbi_call(uint32_t arg0, uint32_t arg1, uint32_t arg2,
    uint32_t arg3, uint32_t arg4, uint32_t arg5,
    uint32_t fid, uint32_t eid)

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
    ret.error = (uint32_t)a0;
    ret.value = (uint32_t)a1;

    return ret;
}

void k_putchar(char ch) {
    sbi_call(ch, 0, 0, 0, 0, 0, 0, 0x01);
}

char k_readchar(void)
{
    int ch = (int)sbi_call(0,0,0,0,0,0,0,2).error;//legasy sbi stopt de char in a1
    
    // SBI fail
    if (ch < 0)
        return 0;

    if (ch == 4)
    {
        exit(4);
    }
    return (char)ch;
}

void exit(uint32_t code){
     k_printf("\n------------------------\nGoodbye\nexit code: %d\n", code);
    sbi_call(0, 0, 0, 0, 0 ,0 ,0,SBI_EXT_SYSTEM_RESET);

    k_panic("\nSBI shutdown failed, check hardware or code....\n", "");
}
#endif
