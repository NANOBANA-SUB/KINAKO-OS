#include "alloc.h"
#include "string.h"
#include "panic.h"

paddr32_t alloc_pages(size_t num_pages) 
{
    static paddr32_t next_free = (paddr32_t)&__free_ram;
    paddr32_t end_addr = (paddr32_t)&__free_ram_end;

    size_t size = num_pages * PAGE_SIZE;

    if (next_free + size > end_addr) 
    {
        panic("Out of memory in alloc_pages");
    }

    paddr32_t addr = next_free;
    next_free += size;

    memset((void *)addr, 0, size);
    return addr;
}

void free_pages(paddr32_t addr, size_t num_pages) 
{
    // 現状は何もしない（簡易実装のため）
}