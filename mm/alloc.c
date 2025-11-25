#include "alloc.h"
#include "string.h"
#include "panic.h"

paddr32_t alloc_pages(size_t n) 
{
    static paddr32_t next_paddr = (paddr32_t) __free_ram;
    paddr32_t paddr = next_paddr;
    next_paddr += n * PAGE_SIZE;

    if (next_paddr > (paddr32_t) __free_ram_end)
        panic("out of memory");

    memset((void *) paddr, 0, n * PAGE_SIZE);
    return paddr;
}

void free_pages(paddr32_t addr, size_t num_pages) 
{
    // 現状は何もしない（簡易実装のため）
}