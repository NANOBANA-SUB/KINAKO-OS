#include "string.h"

void *memset(void *s, int c, size_t n) 
{
    uint8_t *ptr = (uint8_t *)s;
    for (size_t i = 0; i < n; i++) 
    {
        ptr[i] = (uint8_t)c;
    }
    return s;
}

void *memcpy(void *dest, const void *src, size_t n) 
{
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    for (size_t i = 0; i < n; i++) 
    {
        d[i] = s[i];
    }
    return dest;
}

char *strcpy(char *dest, const char *src) 
{
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

size_t strlen(const char *s) 
{
    const char *p = s;
    while (*p) 
    {
        p++;
    }
    return (size_t)(p - s);
}