#ifndef SBI_H
#define SBI_H

#include "types.h"
#include "panic_k.h"

typedef    struct    {
                uint32_t    error;
                uint32_t    value;
 }    sbiret;

sbiret sbi_call(uint32_t arg0, uint32_t arg1, uint32_t arg2,
    uint32_t arg3, uint32_t arg4, uint32_t arg5,
    uint32_t fid, uint32_t eid);

void k_putchar(char ch) ;

char k_readchar(void);

void exit(uint32_t code);

#endif
