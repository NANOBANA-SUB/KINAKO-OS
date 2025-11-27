#include "MMU.h"
#include "panic.h"
#include "alloc.h"
#include "string.h"

extern char __kernel_base[];
extern char __free_ram_end[];

void map_page(uint32_t *tableL1, uint32_t vaddr, paddr32_t paddr, uint32_t flags)
{
    // 仮想アドレスと物理アドレスがページサイズでアラインされているかチェックする
    if (!is_aligned(vaddr, PAGE_SIZE))
        panic("map_page: unaligned vaddr %x\n", vaddr);
        
    if (!is_aligned(paddr, PAGE_SIZE))
        panic("map_page: unaligned paddr %x\n", paddr);

    // 仮想アドレスから仮想ページ番号(vpn1)を取得する
    uint32_t vpn1 = (vaddr >> 22) & 0x3ff;      // riscv32では上位10bitがvpn1となる

    if ((tableL1[vpn1] & PAGE_V) == 0)
    {
        // L1テーブルが存在しないので作成する
        paddr32_t pagetable_paddr = alloc_pages(1);
        tableL1[vpn1] = ((pagetable_paddr / PAGE_SIZE) << 10) | PAGE_V;
    }

    // 2段目のページテーブルにエントリを追加する
    uint32_t vpn0 = (vaddr >> 12) & 0x3ff;
    uint32_t *table0 = (uint32_t *) ((tableL1[vpn1] >> 10) * PAGE_SIZE);
    table0[vpn0] = ((paddr / PAGE_SIZE) << 10) | flags | PAGE_V;
}

void kernel_map_page(uint32_t *table1)
{
    // カーネルのページマッピングを行う
    // マッピングの範囲は__kernel_baseから__free_ram_endまで
    // 現在の実装ではカーネルの仮想アドレスと物理アドレスが一致していることに注意
    for (paddr32_t paddr = (paddr32_t)__kernel_base; paddr < (paddr32_t)__free_ram_end; paddr += PAGE_SIZE)
    {
        map_page(table1, paddr, paddr, PAGE_R | PAGE_W | PAGE_X);
    }
}

void user_map_page(uint32_t *table1, const void *image, size_t image_size)
{
    for (uint32_t off = 0; off < image_size; off += PAGE_SIZE)
    {
        paddr32_t page = alloc_pages(1);

        size_t remaining = image_size - off;
        size_t copy_size = PAGE_SIZE <= remaining ? PAGE_SIZE : remaining;

        // 確保したページに取得したデータをコピーする
        memcpy((void *) page, image + off, copy_size);

        // ページテーブルにマッピングする
        map_page(table1, USER_BASE + off, page, PAGE_U | PAGE_R | PAGE_W | PAGE_X);
    }
}