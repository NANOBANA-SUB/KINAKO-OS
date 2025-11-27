#include "common.h"
#include "console.h"
#include "sbi.h"

void console_putc(char ch) 
{
    sbi_call(ch, 0, 0, 0, 0, 0, 0, 1);
}

void console_write(const char *buf) 
{
    while (*buf) 
    {
        console_putc(*buf++);
    }
}

int console_getchar(void)
{
    struct sbiret ret = sbi_call(0, 0, 0, 0, 0, 0, 0, 2);
    return ret.error;
}