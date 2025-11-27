#include "main.h"

extern char __bss[], __bss_end[], __stack_top[];
extern char __free_ram_start[], __free_ram_end[];

void kernel_main(void) {
    // Initialize BSS to zero
    memset(__bss, 0, ((size_t)__bss_end - (size_t)__bss));
    
    // Register trap handler IMMEDIATELY after BSS initialization
    write_csr(stvec, (uint32_t)switch_trap);
    
    // Initialize memory management
    init_memory();

    k_printf("\n=== Kernel Boot ===\n");
    k_printf("BSS initialized\n");
    k_printf("Trap handler registered at: %p\n", (uint32_t)switch_trap);
    k_printf("Memory system initialized\n");
    k_printf("Free RAM: %p - %p\n", (uint32_t)__free_ram_start, (uint32_t)__free_ram_end);

    // Read back stvec to verify
    uint32_t stvec_value = read_csr(STVEC);
    k_printf("stvec = %p\n", stvec_value);
    
    k_printf("\n=== Testing Trap Handler ===\n");
    k_printf("Triggering illegal instruction trap...\n");
    
    // Trigger a kernel trap by writing to a read-only CSR
    // scountovf (0xDA0) is a supervisor read-only CSR
    __asm__ __volatile__("csrw scountovf, x0");
    
    // If we get here, the trap was handled successfully!
    k_printf("SUCCESS! We survived a kernel trap!\n");
    
    k_printf("\n=== Testing Page Frame Allocator ===\n");
    
    // Test 1: Allocate 3 page frames
    pframe_addr_t pfa0 = pageframalloc(3);
    k_printf("Allocated 3 frames: pfa0 = %p\n", pfa0);
    
    // Test 2: Allocate 1 page frame
    pframe_addr_t pfa1 = pageframalloc(1);
    k_printf("Allocated 1 frame:  pfa1 = %p\n", pfa1);
    
    // Verify addresses are correct (should be 3*4096 = 12288 = 0x3000 apart)
    uint32_t expected_diff = 3 * PAGEFRAMESIZE;
    uint32_t actual_diff = pfa1 - pfa0;
    k_printf("Expected difference: %p\n", expected_diff);
    k_printf("Actual difference:   %p\n", actual_diff);
    
    if (actual_diff == expected_diff) {
        k_printf("Page frame allocation test PASSED!\n");
    } else {
        k_printf("Page frame allocation test FAILED!\n");
    }
    
    // Verify the frames are zeroed
    uint32_t *check_ptr = (uint32_t*)pfa0;
    uint32_t non_zero_count = 0;
    for (uint32_t i = 0; i < (PAGEFRAMESIZE / sizeof(uint32_t)); i++) {
        if (check_ptr[i] != 0) {
            non_zero_count++;
        }
    }
    k_printf("Non-zero words in first frame: %d (should be 0)\n", non_zero_count);
    
    k_printf("\n=== Original Tests ===\n");
    k_printf("Beste %s\n", "Koeien!");
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
    
    char buffer[200];
    k_sprintf(buffer, "test %x\n", 5);
    k_printf("%s\n", buffer);
    
    char *t = "5";
    k_printf("itoa(5) = %s\n", t);
    k_printf("new test: %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", 
             0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
    
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
