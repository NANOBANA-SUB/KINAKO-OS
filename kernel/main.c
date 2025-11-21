#include "common.h"
#include "string.h"
#include "printk.h"

extern char __bss[];
extern char __bss_end[];

void kernel_main() 
{
    // Zero initialize the BSS segment
    size_t bss_size = (size_t)(__bss_end - __bss);
    memset(__bss, 0, bss_size);

    // Kernel main logic here

    printk("Kernel initialized successfully.\n");
    printk("Hello, KINAKO OS!\n");
    
    while (1) 
    {
        __asm__ volatile ("wfi");
    }
}