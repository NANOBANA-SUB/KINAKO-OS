#include "kalloc.h"
#include "alloc.h"
#include "common.h"

paddr32_t kalloc_page(void) 
{
    // 現状は物理ページ＝仮想ページとして扱う簡易実装のため、そのまま物理アドレスを取得する
    return alloc_pages(1);
}

void kfree_page(void* page) 
{
    // 現状は物理ページ＝仮想ページとして扱う簡易実装のため、そのまま物理アドレスを渡す
    paddr32_t paddr = (paddr32_t)page;
    free_pages(paddr, 1);
}