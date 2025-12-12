#ifndef TYPES_H
#define TYPES_H

typedef enum {
    false = 0,
    true = 1
} bool;

typedef unsigned int uintptr_t;
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;
typedef int  int32_t;
#define NULL ((void*)0)

#define PAGEFRAMESIZE 4096

#endif
