#ifndef STRING_H
#define STRING_H

#include "types.h"
// String utilities
uint32_t strlen(const char *str);
int strncmp(const char *s1, const char *s2, uint32_t n);
int starts_with(const char *str, const char *prefix);
uint8_t strcmp(const char *s1, const char *s2);
void strcpy(char *dest, const char *src);
void toLowerCase(char *str);
//utils

static char *strtok_save;

char *strtok(char *str, const char *delim) ;
char *strtok_r(char *str, const char *delim, char **saveptr) ;

// Returns the length of the initial segment of str1 which consists entirely of characters in str2
size_t strspn(const char *str1, const char *str2);

// Returns the length of the initial segment of str1 which consists entirely of characters not in str2
size_t strcspn(const char *str1, const char *str2) ;

#endif
