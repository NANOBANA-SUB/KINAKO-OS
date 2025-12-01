// fs_test_layout.c
// ファイルシステムのテスト用構築

#include "fs.h"
#include "printk.h"
#include "string.h"

#define ROOT_INO  1
#define HELLO_INO 2

// 便利マクロ（すでにあったらそれを使ってOK）
#define IPB (BSIZE / sizeof(struct dinode))  // Inodes Per Block

static inline uint32_t inode_blockno(uint32_t inum)
{
    // inum は 1 起算想定
    return g_sb.inodestart + (inum / IPB);
}

static inline uint32_t inode_block_index(uint32_t inum)
{
    return inum % IPB;
}

void fs_build_test_fs_basic(void)
{
    printk("fs: build test fs\n");

    // 0. ディスク全体を一旦ゼロクリア（任意）
    uint8_t zero[BSIZE];
    memset(zero, 0, sizeof(zero));
    for (uint32_t b = 0; b < DISK_NBLOCKS; b++) {
        bdev_write(b, zero);
    }

    // 1. superblock を構築して block1 に書く
    //   （ログで inodestart=3, bmapstart=10 を使ってたのでそれに合わせる）
    struct superblock sb = {
        .size      = DISK_NBLOCKS,
        .nblocks   = DISK_NBLOCKS - 10,  // 適当。厳密でなくてOK
        .ninodes   = 64,
        .nlog      = 0,
        .logstart  = 2,
        .inodestart = 3,
        .bmapstart  = 10,
    };

    uint8_t buf[BSIZE];
    memset(buf, 0, sizeof(buf));
    memcpy(buf, &sb, sizeof(sb));
    bdev_write(SUPERBLOCK_BLOCKNO, buf);

    g_sb = sb;

    // メモリ上の g_sb もここで更新しておくと fs_load_super 無くても動くが、
    // いったん fs_load_super を通す前提ならここでは書かなくてもOK。

    // 2. /hello のデータブロックを書き込む
    const char *msg = "Hello from inode!\n";
    uint32_t msg_len = (uint32_t)strlen(msg);

    uint32_t hello_data_block = sb.bmapstart + 1;
    memset(buf, 0, sizeof(buf));
    memcpy(buf, msg, msg_len);
    bdev_write(hello_data_block, buf);

    // 3. ルートディレクトリエントリを書き込む
    // block: sb.bmapstart + 2 を dir用に使う
    uint32_t root_dir_block = sb.bmapstart + 2;

    struct dirent de[3];

    // "."
    de[0].inum = ROOT_INO;
    memset(de[0].name, 0, DIRSIZ);
    strncpy(de[0].name, ".", DIRSIZ);

    // ".."（rootなので自分自身）
    de[1].inum = ROOT_INO;
    memset(de[1].name, 0, DIRSIZ);
    strncpy(de[1].name, "..", DIRSIZ);

    // "hello"
    de[2].inum = HELLO_INO;
    memset(de[2].name, 0, DIRSIZ);
    strncpy(de[2].name, "hello", DIRSIZ);

    memset(buf, 0, sizeof(buf));
    memcpy(buf, de, sizeof(de));
    bdev_write(root_dir_block, buf);

    // 4. inode1 (root dir) と inode2 (/hello file) を inode テーブルに書く
    //   ※ ここでは inum は 1 起算の想定

    uint8_t ibuf[BSIZE];

    // 4-1. ROOT_INO (ディレクトリ)
    uint32_t bno = inode_blockno(ROOT_INO);
    uint32_t idx = inode_block_index(ROOT_INO);

    if (bdev_read(bno, ibuf) != 0) 
    {
        printk("fs: failed to read inode block (root)\n");
        return;
    }

    struct dinode *dip = (struct dinode *)ibuf;
    dip += idx;

    memset(dip, 0, sizeof(*dip));
    dip->type  = T_DIR;
    dip->nlink = 1;
    dip->size  = sizeof(de);       // 3エントリ分
    dip->addrs[0] = root_dir_block;

    if (bdev_write(bno, ibuf) != 0) 
    {
        printk("fs: failed to write inode block (root)\n");
        return;
    }

    // 4-2. HELLO_INO (ファイル)
    bno = inode_blockno(HELLO_INO);
    idx = inode_block_index(HELLO_INO);

    if (bdev_read(bno, ibuf) != 0) 
    {
        printk("fs: failed to read inode block (hello)\n");
        return;
    }

    dip = (struct dinode *)ibuf;
    dip += idx;

    memset(dip, 0, sizeof(*dip));
    dip->type  = T_FILE;
    dip->nlink = 1;
    dip->size  = msg_len;
    dip->addrs[0] = hello_data_block;

    if (bdev_write(bno, ibuf) != 0) 
    {
        printk("fs: failed to write inode block (hello)\n");
        return;
    }

    printk("fs: test fs built (root=/, hello=/hello)\n");
}