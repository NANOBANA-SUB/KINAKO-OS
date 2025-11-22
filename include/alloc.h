#pragma once
#include "common.h"

extern char __free_ram[];
extern char __free_ram_end[];

//!
//! ページ単位でメモリを割り当てる
//! @param num_pages 割り当てるページ数
//! @return 割り当てた物理アドレスの先頭、失敗した場合は0
//!
paddr32_t alloc_pages(size_t num_pages);

//!
//! ページ単位でメモリを解放する
//! @param addr 解放する物理アドレスの先頭
//! @param num_pages 解放するページ数
//!
void free_pages(paddr32_t addr, size_t num_pages);