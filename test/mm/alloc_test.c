#include "common.h"
#include "alloc.h"
#include "printk.h"
#include "panic.h"

// ページアロケートテスト
bool test_alloc_pages(void) 
{
    paddr32_t addr1 = alloc_pages(1); // 1ページ(4KB)のメモリを確保
    paddr32_t addr2 = alloc_pages(2); // 2ページ(8KB)のメモリを確保

    if (addr1 == 0 || addr2 == 0) 
    {
        printk("alloc_pages failed: addr1=0x%x, addr2=0x%x\n", addr1, addr2);
        return false;
    }

    printk("alloc_pages succeeded: addr1=0x%x, addr2=0x%x\n", addr1, addr2);
    return true;
}

// アロケートしたページのサイズが正しいか確認するテスト
bool test_allocated_page_size(void)
{
    paddr32_t addr = alloc_pages(3); // 3ページ(12KB)のメモリを確保

    if (addr == 0) 
    {
        printk("alloc_pages failed: addr=0x%x\n", addr);
        return false;
    }

    // 確保したアドレスがページサイズの倍数であることを確認
    if (addr % PAGE_SIZE != 0) 
    {
        printk("Allocated address is not page-aligned: addr=0x%x\n", addr);
        return false;
    }

    printk("Allocated page size test succeeded: addr=0x%x\n", addr);
    return true;
}

// メモリ解放の基本動作を確認するテスト
bool test_free_pages(void)
{
    paddr32_t addr = alloc_pages(2); // 2ページ(8KB)のメモリを確保
    if (addr == 0) 
    {
        printk("alloc_pages failed: addr=0x%x\n", addr);
        return false;
    }
    free_pages(addr, 2); // 確保したメモリを解放
    printk("free_pages succeeded: addr=0x%x\n", addr);
    return true;
}