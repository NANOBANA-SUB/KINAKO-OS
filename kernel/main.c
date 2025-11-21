#include "common.h"
#include "string.h"
#include "printk.h"
#include "panic.h"

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

    panic("This is a test panic message.");
    printk("This line will not be reached.\n");
    while (1) 
    {
        __asm__ volatile ("wfi");
    }
}