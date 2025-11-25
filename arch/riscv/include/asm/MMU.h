#pragma once
#include "common.h"

#define SATP_SV32   (1u << 31)      // satpレジスタのsv32モード設定。sv32はsatpレジスタの上位1ビットが1であるのが条件(riscv32)。
#define PAGE_V    (1 << 0)          // 有効化ビット
#define PAGE_R    (1 << 1)          // 読み込み可能
#define PAGE_W    (1 << 2)          // 書き込み可能
#define PAGE_X    (1 << 3)          // 実行可能
#define PAGE_U    (1 << 4)          // ユーザーモードでアクセス可能

//!
//! ページテーブルを構築する関数
//! @param table1 第一テーブルのポインタ
//! @param vaddr マッピングしたい仮想アドレス
//! @param paddr マッピングしたい物理アドレス
//! @param flags ページの各種フラグ
//!
void map_page(uint32_t *table1, uint32_t vaddr, paddr32_t paddr, uint32_t flags);