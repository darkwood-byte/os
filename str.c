#include "str.h"
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


int k_strcmp(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return *str1 - *str2;  
        }
        str1++;
        str2++;
    }
    return *str1 - *str2; 
}
