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