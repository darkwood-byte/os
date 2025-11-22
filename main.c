#include "main.h"

extern char __bss[], __bss_end[], __stack_top[];

void k_memcpy(void *dest, const void *src, size_t n){
    uint8_t *end = ( uint8_t *)dest;
    uint8_t *start = ( uint8_t *)src;
   
    for( uint32_t index = 0; index <( uint32_t) n; index++){
        *end = *start;
        end++;
        start++;
    }
}

void k_strncpy(char *dest, const char *src, size_t n) {
    size_t i = 0;

    while (i < n && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

     if (i < n) {
            dest[i] = '\0';
    } else {
        dest[n - 1] = '\0';  
    }
}
#define ASCI_UPPERCASE_START 96
#define ASCI_CASE_OFFSET 20
char to_lower_case_char(char *ch){
    (uint8_t)ch;
    if(ch> ASCI_UPPERCASE_START)ch -= ASCI_CASE_OFFSET;
    return (char)ch;
}

int k_strcmp(const char *str1, const char *str2) {
    while (*str1 != '\0' && *str2 != '\0') {
        if (*str1 != *str2) {
            return (*str1 > *str2) ? 1 : -1;
        }
        str1++;
        str2++;
    }
    
    if (*str1 == '\0' && *str2 == '\0') return 0;
    return (*str1 == '\0') ? -1 : 1;
}

void kernel_main(void) {
    k_printf("\nBeste %s\n", "Koeien!");
    k_printf("Yes or no : %c\n", 'y');
    k_printf("39 + 3 = %d\n7 - 15 = %d\n", (39 + 3), (7 - 15));
    k_printf("Doole koe is    : %x\n", 0xDEADBEEF);
    k_printf("Doole koe is ook: %x\n", (unsigned int)3735928559);
    k_printf("Adres van BSS   : %p\n", (unsigned int)__bss);
    char *test1 = "hello world";
    char *test2 = "uhh oh";
    k_strncpy(test2, test1, 20);
    k_printf("%s %s\n", test1, test2);
    k_panic("test %d", 3);
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
