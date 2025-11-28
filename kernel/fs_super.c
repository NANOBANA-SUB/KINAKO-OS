#include "fs.h"
#include "printk.h"
#include "string.h"

struct superblock g_sb;

void fs_load_super(void)
{
    uint8_t buf[BSIZE];
    if (bdev_read(SUPERBLOCK_BLOCKNO, buf) != 0) 
    {
        printk("fs: failed to read superblock\n");
        return;
    }

    // ディスク上は先頭に struct superblock をそのまま置く想定
    memcpy(&g_sb, buf, sizeof(struct superblock));

    printk("fs: superblock loaded: size=%u ninodes=%u inodestart=%u bmapstart=%u\n",
           g_sb.size, g_sb.ninodes, g_sb.inodestart, g_sb.bmapstart);
}

static void fs_test_write_fake_super(void)
{
    // mkfsでimgを作るべきだが、テスト用に直書きを行う
    uint8_t buf[BSIZE];
    memset(buf, 0, sizeof(buf));

    struct superblock sb = 
    {
        .size      = DISK_NBLOCKS,
        .nblocks   = DISK_NBLOCKS - 10,  // 適当
        .ninodes   = 64,
        .nlog      = 0,
        .logstart  = 2,
        .inodestart = 3,
        .bmapstart  = 10,
    };

    memcpy(buf, &sb, sizeof(sb));

    if (bdev_write(SUPERBLOCK_BLOCKNO, buf) != 0) 
    {
        printk("fs: failed to write fake superblock\n");
    }
}

void fs_test_super(void)
{
    printk("fs: superblock test start\n");

    fs_test_write_fake_super();  // テスト用に一旦書く
    fs_load_super();             // 読み込む

    // 読み込んだ g_sb が期待値と一致するかチェック
    if (g_sb.size != DISK_NBLOCKS) 
    {
        printk("fs: superblock test failed: size=%u\n", g_sb.size);
        return;
    }
    if (g_sb.ninodes != 64) 
    {
        printk("fs: superblock test failed: ninodes=%u\n", g_sb.ninodes);
        return;
    }
    // 必要なら他のフィールドもチェック

    printk("fs: superblock test [OK]\n");
}