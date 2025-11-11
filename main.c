#include "main.h"

#define IS_FOLDER(item) ((item)->data == NULL)

#define HEAP_START   0x80400000  // Safe margin after kernel
#define HEAP_END     0x87E00000  // Leave 2MB for stack/other
#define HEAP_SIZE    (HEAP_END - HEAP_START)  // ~122MB available

extern char __bss[], __bss_end[], __stack_top[];
 char *path = "/home";

typedef struct {
    uint32_t size;
    uint8_t *data;
} file;

typedef struct item item;
struct item {
    char *name;
    file *data;
    item *next;
    item *child;
    uint32_t size;
};

// Add these helper functions
uint32_t strlen(const char *str) {
    uint32_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

int strncmp(const char *s1, const char *s2, uint32_t n) {
    for (uint32_t i = 0; i < n; i++) {
        if (s1[i] != s2[i]) {
            return (unsigned char)s1[i] - (unsigned char)s2[i];
        }
        if (s1[i] == '\0') {
            return 0;
        }
    }
    return 0;
}

// Helper function to check if string starts with prefix
int starts_with(const char *str, const char *prefix) {
    while (*prefix) {
        if (*str++ != *prefix++) {
            return 0;
        }
    }
    return 1;
}

// Simple filesystem root - you'll need to initialize this
item *root_directory = NULL;
item *current_directory = NULL;

item *get_current_directory(void) {
    if (current_directory == NULL) {
        // Initialize filesystem if not done yet
        root_directory = malloc(sizeof(item));
        root_directory->name = "root";
        root_directory->data = NULL;  // This is a folder
        root_directory->next = NULL;
        root_directory->child = NULL;
        root_directory->size = 0;
        
        // Create home directory
        item *home = malloc(sizeof(item));
        home->name = "home";
        home->data = NULL;  // This is a folder
        home->next = NULL;
        home->child = NULL;
        home->size = 0;
        
        root_directory->child = home;
        current_directory = home;  // Start in home directory
    }
    return current_directory;
}

// Simple ls - list current directory contents
void ls_simple(void) {
    item *current = get_current_directory();
    item *curr_item = current->child;
    
    if (curr_item == NULL) {
        printk("Directory is empty\n");
        return;
    }
    
    while (curr_item != NULL) {
        if (IS_FOLDER(curr_item)) {
            printk("[%s] ", curr_item->name);
        } else {
            printk("%s ", curr_item->name);
        }
        curr_item = curr_item->next;
    }
    printk("\n");
}

// Complex ls with options
void ls_complex(char *args) {
    if (args == NULL || args[0] == '\0') {
        ls_simple();
        return;
    }
    
    // Parse options
    int show_size = 0;
    int show_type = 0;
    char *arg = args;
    
    while (*arg) {
        if (*arg == '-') {
            arg++;
            while (*arg) {
                if (*arg == 'l') show_size = 1;
                if (*arg == 'F') show_type = 1;
                arg++;
            }
        } else {
            break;
        }
        if (*arg == ' ') arg++;
    }
    
    // List directory
    item *current = get_current_directory();
    item *curr_item = current->child;
    
    if (curr_item == NULL) {
        printk("Directory is empty\n");
        return;
    }
    
    while (curr_item != NULL) {
        if (show_size || show_type) {
            if (IS_FOLDER(curr_item)) {
                printk("[%s]", curr_item->name);
                if (show_type) printk("/");
                if (show_size) printk(" %dB", curr_item->size);
            } else {
                printk("%s", curr_item->name);
                if (show_type) printk("*");
                if (show_size) printk(" %dB", curr_item->size);
            }
            printk("\n");
        } else {
            if (IS_FOLDER(curr_item)) {
                printk("[%s] ", curr_item->name);
            } else {
                printk("%s ", curr_item->name);
            }
        }
        curr_item = curr_item->next;
    }
    
    if (!show_size && !show_type) {
        printk("\n");
    }
}

uint8_t strcmp(char *s1, char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (char)*s1 - (char)*s2;
}

void strcpy(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void toLowerCase(char *str) {
    while (*str) {
        if (*str >= 'A' && *str <= 'Z') {
            *str = *str + ('a' - 'A');
        }
        str++;
    }
}

uint8_t handle_simple(char *input){
    if (!strcmp(input, "cd")){
        path = "/home";
        return 1;
    }
    else  if (!strcmp(input, "root")){
        path = "";
        return 1;
    }
    else if (!strcmp(input, "ls")) {
        ls_simple();
        return 1;
    }
    else if (!strcmp(input, "quit") || !strcmp(input, "exit")) {
        printk("Goodbye!\n");

        __asm__  __volatile__ ("li a7, 0x53525354\n"  // SBI SRST extension ID
                      "li a6, 0x0\n"         // SBI_EXT_SRST
                      "li a0, 0x0\n"         // SBI_SRST_RESET_TYPE_SHUTDOWN
                      "li a1, 0x0\n"         // SBI_SRST_RESET_REASON_NO_REASON
                      "ecall");
        return 1;
    }
    else if (!strcmp(input, "cory")){
        print_cory();
        return 1;
    }
    else if (!strcmp(input, "logo")){
        print_start();
        return 1;
    }
    else if (!strcmp(input, "mem")){
        printk("\n------------------\nused memory: %d\nfree memory: %d\n------------------", get_used_memory(), get_free_memory());
        return 1;
    }
    return 0;
}

void touch(char *name){
    if (name == NULL || name[0] == '\0') {
        printk("touch: missing filename\n");
        return;
    }

    // Check if file already exists in current directory
    item *current = get_current_directory();
    item *curr_item = current->child;
    
    while (curr_item != NULL) {
        if (!IS_FOLDER(curr_item) && !strcmp(curr_item->name, name)) {
            printk("touch: file '%s' already exists\n", name);
            return;
        }
        curr_item = curr_item->next;
    }

    // Create new file entry
    item *new_file = malloc(sizeof(item));
    if (new_file == NULL) {
        printk("touch: insufficient memory\n");
        return;
    }

    // Allocate and copy filename
    new_file->name = malloc(strlen(name) + 1);
    if (new_file->name == NULL) {
        free(new_file);
        printk("touch: insufficient memory\n");
        return;
    }
    strcpy(new_file->name, name);

    // Initialize file structure
    file *file_data = malloc(sizeof(file));
    if (file_data == NULL) {
        free(new_file->name);
        free(new_file);
        printk("touch: insufficient memory\n");
        return;
    }

    file_data->size = 0;
    file_data->data = NULL;
    
    new_file->data = file_data;
    new_file->size = 0;
    new_file->next = NULL;
    new_file->child = NULL;

    // Add to current directory
    if (current->child == NULL) {
        current->child = new_file;
    } else {
        // Add to end of child list
        item *last = current->child;
        while (last->next != NULL) {
            last = last->next;
        }
        last->next = new_file;
    }

    printk("Created file '%s'\n", name);
}

void handle_complex(char *input){
    if (strncmp(input, "touch ", 6) == 0) {
        touch(input + 6);
        return;
    }
    else if (starts_with(input, "ls ")) {
        ls_complex(input + 3);
        return;
    }
    printk("Unknown command: %s\n", input);
}

void bronx(void) {
    char input[128];
    uint32_t len = get_uart_input(input, sizeof(input));
    toLowerCase(input);
    if (!handle_simple(input))handle_complex(input);
    (void)len;
    return;
}



void kernel_main(void) {
    memset(__bss, 0, ((size_t)__bss_end - (size_t)__bss));
    
    memory_init((void*)HEAP_START, HEAP_SIZE);

   print_start();
    
    while (1) {
        printk("\n~%s: ",path);
        bronx();
    }
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
