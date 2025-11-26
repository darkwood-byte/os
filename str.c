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

void k_sprintf(char* dest,const char* format, ...){
    va_list_rv args;
    va_start_rv(args, format);

    char buf[32];
    const char *p = format;
    
    (void)va_arg_rv(args, uint32_t);

    while (*p) {
        if (*p != '%') {
            *dest = *p;
            p++;
            dest++;
            continue;
        }

        p++;

        switch (*p) {
        case 'd': {
            int32_t val = va_arg_rv(args, int32_t);
            itoa(val, buf, 10);
            for (char *s = buf; *s; s++){
                *dest = *s;
                dest++;
            }
            break;
        }
        case 'u': {
            uint32_t val = va_arg_rv(args, uint32_t);
            utoa(val, buf, 10);
            for (char *s = buf; *s; s++){
                *dest = *s;
                dest++;
            }
            break;
        }

        case 'x': {
            uint32_t val = va_arg_rv(args, uint32_t);
            *dest = '0';
            dest++;
            *dest = 'x';
            dest++;
            utoa(val, buf, 16);
            for (char *s = buf; *s; s++){
                *dest = *s;
                dest++;
            }
            break;
        }

        case 'p': {
            *dest = '0';
            dest++;
            *dest = 'x';
            dest++;
            uint32_t val = va_arg_rv(args, uint32_t);
            utoa(val, buf, 16);
            for (char *s = buf; *s; s++){
                *dest = *s;
                dest++;
            }
            break;
        }

        case 's': {
            char *str = va_arg_rv(args, char*);
            if (!str) str = "(null)";
            for (; *str; str++){
                *dest = *str;
                dest++;
            }
            break;
        }

        case 'c': {
            char c = (char)va_arg_rv(args, int);
           *dest = c;
            dest++;
            break;
        }

        case '%': {
           *dest = '%';
            dest++;
            break;
        }

        default: {
            *dest = '%';
            dest++;
            *dest = *p;
            dest++;
            break;
        }
        }

        p++;
    }
    *dest = '\0';
    va_end_rv(args);
}

