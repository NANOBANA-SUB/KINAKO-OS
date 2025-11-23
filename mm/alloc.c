#include "alloc.h"
#include "string.h"
#include "panic.h"

paddr32_t alloc_pages(size_t num_pages) 
{
    static paddr32_t next_free = (paddr32_t)&__free_ram;
    paddr32_t addr = next_free;
    paddr32_t end_addr = (paddr32_t)&__free_ram_end;
    next_free += num_pages * 4096;

    size_t size = num_pages * 4096; // ページサイズは4KB

    if (next_free + size > end_addr) 
    {
        // メモリ不足
        panic("Out of memory in alloc_pages");
    }

    memset((void *)addr, 0, size); // 確保したメモリをゼロ初期化
    return addr;
}

void free_pages(paddr32_t addr, size_t num_pages) 
{
    // 現状は何もしない（簡易実装のため）
}