#pragma once
#include "common.h"

/* ブロックデバイス層 */
#define BSIZE 1024          // ブロックサイズ
#define DISK_NBLOCKS 1024   // 仮想ディスクのブロック数(1 * 1024 * 1024 = 1MB)

/* superblock */
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

/* inode */
// inode type
#define T_NONE  0
#define T_DIR   1
#define T_FILE  2

#define NDIRECT   12
#define NINDIRECT (BSIZE / sizeof(uint32_t))
#define MAXFILE   (NDIRECT + NINDIRECT)

struct dinode 
{
    uint16_t type;      // T_FILE, T_DIR, ...
    uint16_t major;
    uint16_t minor;
    uint16_t nlink;
    uint32_t size;
    uint32_t addrs[NDIRECT + 1]; // 直接ブロック + 1個の間接ブロック
};

// メモリ上の inode
struct inode 
{
    uint32_t dev;
    uint32_t inum;      // ユニークなナンバー
    int ref;            // 参照回数
    int valid;          // ディスクから読み込んだか

    uint16_t type;
    uint16_t major;
    uint16_t minor;
    uint16_t nlink;
    uint32_t size;
    uint32_t addrs[NDIRECT + 1];
};

#define NINODE 32

/* ディレクトリ構造 */
#define DIRSIZ 14   // ディレクトリ名称のサイズ
struct dirent
{
    uint16_t inum;
    char name[DIRSIZ];
};


/* ブロックデバイスAPI */
//!
//! 指定したブロックナンバーのデータをブロックサイズ分だけ読み込む
//! 読み込みに成功した場合、入力元であるdstにデータが入る
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

/* スーパーブロックAPI */

//!
//! superblockのロードを行う
//!
void fs_load_super(void);

/* inode API */

//!
//! inodeの初期化
//!
void inode_init(void);

//!
//! inumからinode構造体を取得する
//!
struct inode *iget(uint32_t inum);

//!
//! ディスクからdinodeを読んでinodeに展開する
//!
void ilock(struct inode *ip);

//!
//! 現状未実装
//!
void iunlock(struct inode *ip);

//!
//! 指定したinodeからデータを読み出す
//! @param ip inodeのポインタ
//! @param dst 読み込み先データへのポインタ
//! @param off オフセット
//! @param n 読み込むサイズ
//! @return 読み取ったバイト数
//!
int readi(struct inode *ip, void *dst, uint32_t off, uint32_t n);

//!
//! ディレクトリから特定のファイル名の検索を行う
//! @param dp ディレクトリのinode
//! @param name 探したいファイル名
//! @param poff 見つかった位置(NULL可)
//! @return 該当のinode
//!
struct inode *dirlookup(struct inode *dp, const char *name, uint32_t *poff);

//!
//! ディレクトリを順番にたどりながらinodeを探す関数
//! @param path パス名
//!
struct inode *namei(const char *path);

/* FSの初期化およびテスト */
void fs_init(void);
void fs_test_block(void);
void fs_test_super(void);
void fs_test_inode(void);
void fs_test_namei(void);