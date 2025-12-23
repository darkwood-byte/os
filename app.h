#ifndef APP_H
#define APP_H
#include "types.h"
#include "panic_k.h"
#include "program_k.h"

#define MAX_APPS 10

typedef struct
{
    char name[12];
    uint32_t start;
    uint32_t size;
}app;

extern app app_list[MAX_APPS];

//voegt een app toe aan het telfoon boekje van de kernel
uint32_t init_app(char name[12], char start[], char size[]);
//start een app
uint32_t start_app(uint32_t app_id);

#endif
