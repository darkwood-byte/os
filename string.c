#ifndef STRING_C
#define STRING_C

#include "string.h"

// String utilities
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

int starts_with(const char *str, const char *prefix) {
    while (*prefix) {
        if (*str++ != *prefix++) {
            return 0;
        }
    }
    return 1;
}

uint8_t strcmp(const char *s1, const char *s2) {
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

// Static variable to maintain state between calls
static char *strtok_save = NULL;

char *strtok(char *str, const char *delim) {
    return strtok_r(str, delim, &strtok_save);
}

char *strtok_r(char *str, const char *delim, char **saveptr) {
    // If str is NULL, use the saved pointer from previous call
    if (str == NULL) {
        str = *saveptr;
        if (str == NULL) {
            return NULL;
        }
    }
    
    // Skip leading delimiters
    str += strspn(str, delim);
    
    // If we're at the end of the string, return NULL
    if (*str == '\0') {
        *saveptr = NULL;
        return NULL;
    }
    
    // Find the end of the token
    char *end = str + strcspn(str, delim);
    
    // If we found a delimiter, replace it with null terminator and save position
    if (*end != '\0') {
        *end = '\0';
        *saveptr = end + 1;
    } else {
        *saveptr = NULL;
    }
    
    return str;
}

// Returns the length of the initial segment of str1 which consists entirely of characters in str2
size_t strspn(const char *str1, const char *str2) {
    const char *s1, *s2;
    for (s1 = str1; *s1 != '\0'; s1++) {
        for (s2 = str2; *s2 != '\0'; s2++) {
            if (*s1 == *s2) {
                break;
            }
        }
        if (*s2 == '\0') {
            break;
        }
    }
    return (size_t)(s1 - str1);
}

// Returns the length of the initial segment of str1 which consists entirely of characters not in str2
size_t strcspn(const char *str1, const char *str2) {
    const char *s1, *s2;
    for (s1 = str1; *s1 != '\0'; s1++) {
        for (s2 = str2; *s2 != '\0'; s2++) {
            if (*s1 == *s2) {
                break;
            }
        }
        if (*s2 != '\0') {
            break;
        }
    }
    return (size_t)(s1 - str1);
}

#endif
