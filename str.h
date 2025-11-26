#ifndef STR_H
#define STR_H

#include "types.h"
#include "variadic.h"
#include "oa.h"

void k_strncpy(char *dest, const char *src, size_t n) ;

int k_strcmp(const char *str1, const char *str2) ;

void k_sprintf(char* dest,const char* format, ...);

#endif
