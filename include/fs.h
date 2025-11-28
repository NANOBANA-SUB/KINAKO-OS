#pragma once
#include "common.h"

#define BSIZE 1024          // ブロックサイズ
#define DISK_NBLOCKS 1024   // 仮想ディスクのブロック数(1 * 1024 * 1024 = 1MB)

struct superblock 
{
    uint32_t size;       // 全ブロック数
    uint32_t nblocks;    // データブロック総数
    uint32_t ninodes;    // inode 数
    uint32_t nlog;       
    uint32_t logstart;
    uint32_t inodestart; // inode テーブルの開始ブロック
    uint32_t bmapstart;  // ビットマップの開始ブロック
};

#define SUPERBLOCK_BLOCKNO 1

extern struct superblock g_sb;

// ブロックデバイスAPI
//!
//! 指定したブロックナンバーのデータをブロックサイズ分だけ読み込む
//! @param blockno ブロックナンバー
//! @param dst 読み込みデータへのポインタ
//! @return 読み込みに成功した場合は0, 失敗した場合は-1
//!
int bdev_read(uint32_t blockno, void *dst);

//!
//! 指定したブロックナンバーのデータをブロックサイズ分だけ書き出す
//! @param blockno ブロックナンバー
//! @param src 書き出し元のポインタ
//! @return 書き込みに成功した場合は0, 失敗した場合は-1
//!
int bdev_write(uint32_t blockno, const void *src);

// スーパーブロックAPI
void fs_load_super(void);

// FSの初期化およびテスト
void fs_init(void);
void fs_test_block(void);
void fs_test_super(void);