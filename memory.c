#ifndef MEMORY_C
#define MEMORY_C

#include "memory.h"

// Block header structure
typedef struct block_header {
    size_t size;
    struct block_header *next;
    uint8_t used;
} block_header_t;

#define BLOCK_HEADER_SIZE sizeof(block_header_t)
#define MIN_BLOCK_SIZE 16
#define ALIGNMENT 8

static block_header_t *free_list = NULL;
static void *heap_start = NULL;
static size_t heap_size = 0;

// Helper function to align sizes
static inline size_t align_size(size_t size) {
    return (size + (ALIGNMENT - 1)) & (uint32_t)~(ALIGNMENT - 1);
}

// Initialize memory manager
void memory_init(void *start, size_t size) {
    heap_start = start;
    heap_size = size;
    
    // Initialize the first free block
    free_list = (block_header_t *)start;
    free_list->size = size - BLOCK_HEADER_SIZE;
    free_list->next = NULL;
    free_list->used = 0;
}

// Find a free block that fits (best fit algorithm)
static block_header_t *find_free_block(size_t size) {
    block_header_t *current = free_list;
    block_header_t *best_fit = NULL;
    
    while (current) {
        if (!current->used && current->size >= size) {
            // Best fit strategy
            if (!best_fit || current->size < best_fit->size) {
                best_fit = current;
            }
        }
        current = current->next;
    }
    
    return best_fit;
}

// Split a block if it's large enough
static void split_block(block_header_t *block, size_t size) {
    if (block->size >= size + BLOCK_HEADER_SIZE + MIN_BLOCK_SIZE) {
        block_header_t *new_block = (block_header_t *)((uint8_t *)block + BLOCK_HEADER_SIZE + size);
        new_block->size = block->size - size - BLOCK_HEADER_SIZE;
        new_block->next = block->next;
        new_block->used = 0;
        
        block->size = size;
        block->next = new_block;
    }
}

// Merge adjacent free blocks
static void merge_free_blocks(void) {
    block_header_t *current = free_list;
    
    while (current && current->next) {
        if (!current->used && !current->next->used &&
            (uint8_t *)current + BLOCK_HEADER_SIZE + current->size == (uint8_t *)current->next) {
            // Merge current with next
            current->size += BLOCK_HEADER_SIZE + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

// malloc implementation
void *malloc(size_t size) {
    if (size == 0 || !free_list) {
        return NULL;
    }
    
    // Align the requested size
    size_t aligned_size = align_size(size);
    
    // Find a suitable free block
    block_header_t *block = find_free_block(aligned_size);
    if (!block) {
        return NULL; // No suitable block found
    }
    
    // Split the block if possible
    split_block(block, aligned_size);
    
    // Mark as used
    block->used = 1;
    
    // Return pointer to data area
    return (void *)((uint8_t *)block + BLOCK_HEADER_SIZE);
}

// free implementation
void free(void *ptr) {
    if (!ptr) {
        return;
    }
    
    // Get block header from data pointer
    block_header_t *block = (block_header_t *)((uint8_t *)ptr - BLOCK_HEADER_SIZE);
    
    // Mark as free
    block->used = 0;
    
    // Merge adjacent free blocks
    merge_free_blocks();
}

// calloc implementation
void *calloc(size_t num, size_t size) {
    size_t total_size = num * size;
    void *ptr = malloc(total_size);
    
    if (ptr) {
        // Zero the memory
        uint8_t *p = (uint8_t *)ptr;
        for (size_t i = 0; i < total_size; i++) {
            p[i] = 0;
        }
    }
    
    return ptr;
}

// realloc implementation
void *realloc(void *ptr, size_t size) {
    if (!ptr) {
        return malloc(size);
    }
    
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    
    block_header_t *block = (block_header_t *)((uint8_t *)ptr - BLOCK_HEADER_SIZE);
    
    // If the current block is large enough, return the same pointer
    if (block->size >= size) {
        split_block(block, align_size(size));
        return ptr;
    }
    
    // Allocate new block and copy data
    void *new_ptr = malloc(size);
    if (new_ptr) {
        // Copy old data (up to minimum of old and new size)
        size_t copy_size = block->size < size ? block->size : size;
        uint8_t *old_p = (uint8_t *)ptr;
        uint8_t *new_p = (uint8_t *)new_ptr;
        
        for (size_t i = 0; i < copy_size; i++) {
            new_p[i] = old_p[i];
        }
        
        free(ptr);
    }
    
    return new_ptr;
}

// Get free memory statistics
size_t get_free_memory(void) {
    size_t free = 0;
    block_header_t *current = free_list;
    
    while (current) {
        if (!current->used) {
            free += current->size;
        }
        current = current->next;
    }
    
    return free;
}

// Get used memory statistics
size_t get_used_memory(void) {
    size_t used = 0;
    block_header_t *current = free_list;
    
    while (current) {
        if (current->used) {
            used += current->size;
        }
        current = current->next;
    }
    
    return used;
}

#endif
