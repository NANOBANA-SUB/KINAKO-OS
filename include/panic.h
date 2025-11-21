#pragma once
#include "printk.h"

#define panic(fmt, ...)                     \
    do {                                     \
        printk("PANIC at %s:%d: " fmt "\n", \
               __FILE__, __LINE__, ##__VA_ARGS__); \
        while (1) { __asm__ volatile ("wfi"); } \
    } while (0)