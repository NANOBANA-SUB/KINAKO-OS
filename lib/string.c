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

int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char  *p1 = (const unsigned char *)s1;
    const unsigned char  *p2 = (const unsigned char *)s2;

    while (n-- > 0) 
    {
        if (*p1 != *p2)
            return (*p1 - *p2);
        p1++;
        p2++;
    }
    return (0);
}

char *strcpy(char *dest, const char *src) 
{
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n) 
{
    char *d = dest;
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) 
    {
        d[i] = src[i];
    }
    for (; i < n; i++) 
    {
        d[i] = '\0';
    }
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

int strcmp(const char *s1, const char *s2) 
{
    while (*s1 && *s2) {
        if (*s1 != *s2)
            break;
        s1++;
        s2++;
    }

    return *(unsigned char *)s1 - *(unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    const char *p1 = s1;
    const char *p2 = s2;

    while (n-- > 0)
    {
        if (*p1 != *p2)
            return (*(unsigned char *)p1 - *(unsigned char *)p2);

        if (*p1 == '\0')
            return 0;

        p1++;
        p2++;
    }
    return 0;
}