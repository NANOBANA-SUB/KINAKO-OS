#pragma once
#include "common.h"

//!
//! 1ページ（4KB）を割り当てる
//! @return 割り当てたページの仮想アドレスを返す
//!
void* kalloc_page(void);

//!
//! 1ページ（4KB）を解放する
//! @param page 解放するページの仮想アドレスへのポインタ
//!
void kfree_page(void* page);