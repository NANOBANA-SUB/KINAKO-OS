#include "user.h"
#include "syscall.h"

extern char __stack_top[];

__attribute__((noreturn)) void exit(void) 
{
    for (;;);
}

void putchar(char ch) 
{
    syscall(SYS_PUTCHAR, ch, 0, 0);
}

int getchar(void)
{
    return syscall(SYS_GETCHAR, 0, 0, 0);
}

void puts(const char *buf)
{
    while(*buf)
    {
        putchar(*buf++);
    }
}

int open(const char *path, int omode)
{
    return syscall(SYS_OPEN, (uintptr_t)path, (uintptr_t)omode, 0);
}

int read(int fd, void *buf, int n)
{
    return syscall(SYS_READ, (uintptr_t)fd, (uintptr_t)buf, (uintptr_t)n);
}

int close(int fd)
{
    return syscall(SYS_CLOSE, (uintptr_t)fd, 0, 0);
}

__attribute__((section(".text.start")))
__attribute__((naked))
void start(void) 
{
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n"
        "call main\n"
        "call exit\n" ::[stack_top] "r"(__stack_top));
}