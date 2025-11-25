#include "kalloc.h"
#include "alloc.h"
#include "common.h"

void* kalloc_page(void) 
{
    // 現状は物理ページ＝仮想ページとして扱う簡易実装のため、そのまま物理アドレスを取得する
    paddr32_t alloc = alloc_pages(1);
    return (void*) alloc;
}

void kfree_page(void* page) 
{
    // 現状は物理ページ＝仮想ページとして扱う簡易実装のため、そのまま物理アドレスを渡す
    paddr32_t paddr = (paddr32_t)page;
    free_pages(paddr, 1);
}