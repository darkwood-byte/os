#include "main.h"

extern char __bss[], __bss_end[], __stack_top[];

void kernel_main(void) {
    memset(__bss, 0, ((size_t)__bss_end - (size_t)__bss));

    k_printf("\nBeste %s\n", "Koeien!");
    k_printf("Yes or no : %c\n", 'y');
    k_printf("39 + 3 = %d\n7 - 15 = %d\n", (39 + 3), (7 - 15));
    k_printf("Doole koe is    : %x\n", 0xDEADBEEF);
    k_printf("Doole koe is ook: %x\n", (unsigned int)3735928559);
    k_printf("Adres van BSS   : %p\n", (unsigned int)__bss);

    int a =  42;
    int b = 7;
    k_memcpy(&b, &a, sizeof(int));
    k_printf("a = %d b = %d\n", a, b);

    char *src = "Boek vooruitgang!";
    char scr_kopie[20] = "uhh oh";
    k_strncpy(scr_kopie, src, 20);
    k_printf("inhoud van scr = %s inhoud van scr_kopie = %s\n", src, scr_kopie);

    char *str1 = "JeMoeder";
    char *str2 = "JeMoeder";
    int cmp1 = k_strcmp(str1, str2);
    k_printf("k_strcmp(\"%s\", \"%s\") = %d\n", str1, str2, cmp1);

    str1 = "JeMoeder";
    str2 = "Oxelhaer";
    int cmp2 = k_strcmp(str1, str2);
    k_printf("k_strcmp(\"%s\", \"%s\") = %d\n", str1, str2, cmp2);

    k_panic("boot-up succeeded, what we do?!", "");
    while(1);
}


__attribute__((section(".text.boot")))
__attribute__((naked))
void boot(void) {
    __asm__ __volatile__(
        "add sp, zero, %[stack_top]\n"
        "jal zero, kernel_main\n"
        :
        : [stack_top] "r" (__stack_top)
        : "memory"
    );
}
