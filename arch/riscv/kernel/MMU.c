#include "MMU.h"
#include "panic.h"
#include "alloc.h"

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