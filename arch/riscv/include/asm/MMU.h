#pragma once
#include "common.h"

#define SATP_SV32 (1u << 31)
#define PAGE_V    (1 << 0)   // 有効化ビット
#define PAGE_R    (1 << 1)   // 読み込み可能
#define PAGE_W    (1 << 2)   // 書き込み可能
#define PAGE_X    (1 << 3)   // 実行可能
#define PAGE_U    (1 << 4)   // ユーザーモードでアクセス可能
#define PAGE_G    (1u << 5)
#define PAGE_A    (1u << 6)
#define PAGE_D    (1u << 7)

//!
//! 1ページ分のマッピングを構築する
//!
//! SV32モードの2段ページテーブルを用いて、指定した仮想アドレス vaddr に
//! 物理アドレス paddr を対応付ける。必要に応じて下位(L0)ページテーブルを
//! 動的に確保してエントリを設定する。
//!
//! @param table1  1段目(L1)ページテーブルの先頭ポインタ（ルートページテーブル）
//! @param vaddr   マッピングしたい仮想アドレス（ページ境界にアラインされていること）
//! @param paddr   対応付ける物理アドレス（ページ境界にアラインされていること）
//! @param flags   PTEに設定するフラグ（R/W/X/V 等）
//!
//! @note SV32モードでは二段構造のページテーブルになっており、本関数は
//!       vaddr から VPN[1], VPN[0] を計算して L1/L0 の PTE を設定する。
//!
void map_page(uint32_t *table1, uint32_t vaddr, paddr32_t paddr, uint32_t flags);

//!
//! 指定されたページテーブル上にカーネル空間をマッピングする
//!
//! カーネルコード・データ・スタック、必要なデバイス領域など、カーネルが
//! 利用する共通のアドレス空間を、指定されたページテーブル上に構築する。
//! 新しく生成したプロセス用ページテーブルに対して呼び出すことを想定している。
//!
//! @param pagetable 1段目(L1)ページテーブルの先頭ポインタ
//!
void map_kernel_space(uint32_t *pagetable);

void dump_pte(uint32_t *pagetable, vaddr32_t vaddr);