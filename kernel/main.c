#include "common.h"
#include "string.h"
#include "console.h"

extern char __bss[];
extern char __bss_end[];

void kernel_main() 
{
    // Zero initialize the BSS segment
    size_t bss_size = (size_t)(__bss_end - __bss);
    memset(__bss, 0, bss_size);

    // Kernel main logic here

    const char *msg = "Hello, KINAKO OS!\n";
    console_write(msg);

    while (1) 
    {
        __asm__ volatile ("wfi");
    }
}