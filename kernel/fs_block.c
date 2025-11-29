#include "fs.h"
#include "printk.h"
#include "string.h"

// グローバル変数
static uint8_t g_disk[DISK_NBLOCKS * BSIZE];    // 単純な配列をディスク扱いにする

// APIの実装
int bdev_read(uint32_t blockno, void *dst)
{
    if (blockno >= DISK_NBLOCKS)
    {
        // blocknoがディスクの最大ブロック数を超えた場合はエラー
        printk("bdev_read: invalid blockno=%u\n", blockno);
        return -1;
    }

    // 指定ナンバーのブロックサイズ分だけ読み込む
    memcpy(dst, &g_disk[blockno * BSIZE], BSIZE);
    return 0;
}

int bdev_write(uint32_t blockno, const void *src)
{
    if (blockno >= DISK_NBLOCKS)
    {
        // blocknoがディスクの最大ブロック数を超えた場合はエラー
        printk("bdev_read: invalid blockno=%u\n", blockno);
        return -1;
    }

    // ブロックデータをディスクに書き込む
    memcpy(&g_disk[blockno * BSIZE], src, BSIZE);
    return 0;
}

void fs_init(void)
{
    printk("fs: init\n");
    
    fs_test_block();
    fs_test_super();
    fs_test_inode();
    fs_test_namei();
}

void fs_test_block(void)
{
    printk("fs: block test start\n");

    uint8_t wbuf[BSIZE];
    uint8_t rbuf[BSIZE];

    // 適当にパターンを書く
    for (int i = 0; i < BSIZE; i++) 
    {
        wbuf[i] = (uint8_t)(i & 0xFF);
    }

    // いくつかのブロックに書き込み→読み戻し
    for (uint32_t bno = 0; bno < 4; bno++) 
    {
        if (bdev_write(bno, wbuf) != 0) 
        {
            printk("fs: block test write failed at %u\n", bno);
            return;
        }

        memset(rbuf, 0, sizeof(rbuf));

        if (bdev_read(bno, rbuf) != 0) 
        {
            printk("fs: block test read failed at %u\n", bno);
            return;
        }

        if (memcmp(wbuf, rbuf, BSIZE) != 0) 
        {
            printk("fs: block test mismatch at %u\n", bno);
            return;
        }
    }

    printk("fs: block test [OK]\n");
}