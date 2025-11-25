#pragma once
#include "common.h"

//!
//! 1ページ（4KB）を割り当てる
//! @return 割り当てたページの物理アドレス
//!
paddr32_t kalloc_page(void);

//!
//! 1ページ（4KB）を解放する
//! @param page 解放するページの物理アドレスへのポインタ
//!
void kfree_page(void* page);