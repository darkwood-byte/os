typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;
typedef int  int32_t;
typedef unsigned int uintptr_t;
#define NULL ((void*)0)

extern char __bss[], __bss_end[], __stack_top[];

typedef    struct    {
                uint32_t    error;
                uint32_t    value;
 }    sbiret;

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

void putchar(char ch) {
    sbi_call(ch, 0, 0, 0, 0, 0, 0, 0x01, ch);
}

void printk(const char *str) {
    // Loop door de string tot het null karakter (\0)
    while (*str != '\0') {
        putchar(*str);  // Print huidig karakter
        str++;          // Ga naar volgend karakter
    }

}

void kernel_main(void) {
    printk("helo world");
    while(1);
}

__attribute__((section(".text.boot")))
__attribute__((naked))
void boot(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n"
        "j kernel_main\n"
        :
        : [stack_top] "r" (__stack_top)
    );
}
