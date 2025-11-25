#include "MMU.h"
#include "kalloc.h"
#include "panic.h"

extern char __kernel_base[];        // カーネルが利用するアドレスの先頭
extern char __free_ram_end[];       // カーネルが利用するアドレスの最後

void map_page(uint32_t *table1, uint32_t vaddr, paddr32_t paddr, uint32_t flags) {
    if (!is_aligned(vaddr, PAGE_SIZE))
        panic("unaligned vaddr %x", vaddr);

    if (!is_aligned(paddr, PAGE_SIZE))
        panic("unaligned paddr %x", paddr);

    uint32_t vpn1 = (vaddr >> 22) & 0x3ff;
    if ((table1[vpn1] & PAGE_V) == 0) {
        // 1段目のページテーブルが存在しないので作成する
        uint32_t pt_paddr = kalloc_page();
        table1[vpn1] = ((pt_paddr / PAGE_SIZE) << 10) | PAGE_V;
    }

    // 2段目のページテーブルにエントリを追加する
    uint32_t vpn0 = (vaddr >> 12) & 0x3ff;
    uint32_t *table0 = (uint32_t *) ((table1[vpn1] >> 10) * PAGE_SIZE);
    table0[vpn0] = ((paddr / PAGE_SIZE) << 10) | flags | PAGE_V;
}

void map_kernel_space(uint32_t *pagetable)
{
    uint32_t start = (uint32_t)__kernel_base;
    uint32_t end   = (uint32_t)__free_ram_end;

    for (uint32_t pa = start; pa < end; pa += PAGE_SIZE) {
        uint32_t va = pa;  // identity mapping 前提
        map_page(pagetable, va, (paddr32_t)pa, PAGE_R | PAGE_W | PAGE_X);
    }
}

void dump_pte(uint32_t *pagetable, uint32_t va)
{
    uint32_t vpn1 = (va >> 22) & 0x3ff;
    uint32_t vpn0 = (va >> 12) & 0x3ff;

    uint32_t pde = pagetable[vpn1];

    printk("[dump_pte] va=%p vpn1=%u vpn0=%u\n", (void*)va, vpn1, vpn0);
    printk("  L1[pde]=0x%08x\n", pde);

    if (!(pde & PAGE_V)) {
        printk("  -> L1 entry not valid\n");
        return;
    }

    uint32_t ppn_l0 = pde >> 10;
    uint32_t *table0 = (uint32_t *)(ppn_l0 * PAGE_SIZE);

    uint32_t pte = table0[vpn0];
    printk("  L0[pte]=0x%08x (ppn=%u flags=0x%x)\n",
           pte, pte >> 10, pte & 0x3ff);

    if (!(pte & PAGE_V)) {
        printk("  -> L0 entry not valid\n");
    }
}