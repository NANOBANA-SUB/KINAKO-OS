#include "fs.h"
#include "printk.h"
#include "string.h"

#define NFILE 64    // とりあえず固定長テーブル

static struct file g_file_table[NFILE];

void file_init(void)
{
    memset(&g_file_table, 0, sizeof(g_file_table));
}

struct file *file_alloc(void)
{
    for (int i = 0; i < NFILE; i++)
    {
        if (g_file_table[i].ref == 0)
        {
            g_file_table[i].ref = 1;
            g_file_table[i].type = FD_NONE;
            return &g_file_table[i];
        }
    }
    printk("file_alloc: no free file\n");
    return NULL;
}

void file_close(struct file *f)
{
    if (f == NULL)
        return;

    if (f->ref < 1)
    {
        printk("file_close: ref < 1\n");
        return;
    }

    f->ref--;
    if (f->ref > 0)
        return;

    if (f->type == FD_INODE)
    {
        // ここにきたらクローズするが、現状は未実装
    }

    // クリア
    f->type = FD_NONE;
    f->ip = NULL;
    f->readable = 0;
    f->writable = 0;
    f->off = 0;
}

int file_read(struct file *f, void *dst, uint32_t n)
{
    if (f->readable == 0)
        return -1;

    if (f->type == FD_INODE)
    {
        ilock(f->ip);
        int r = readi(f->ip, dst, f->off, n);

        if (r > 0)
        {
            f->off += r;
        }
        iunlock(f->ip);
        return r;
    }
    return -1;
}

int file_write(struct file *f, const void *src, uint32_t n)
{
    // まだ書き込みはテストしないので、とりあえず未実装
    (void)f;
    (void)src;
    (void)n;
    printk("file_write: not implemented\n");
    return -1;
}

struct file *file_open(const char *path, int omode)
{
    struct inode *ip = namei(path);
    if (ip == NULL)
    {
        printk("file_open: namei failed for %s\n", path);
        return NULL;
    }
    // 今は read-only を想定
    if ((omode & (O_WRONLY | O_RDWR)) != 0) 
    {
        printk("file_open: write mode not supported yet\n");
        return NULL;
    }

    // ファイルを確保して設定する
    struct file *fp = file_alloc();
    if (fp == NULL)
    {
        return NULL;
    }

    fp->type = FD_INODE;
    fp->ip = ip;
    fp->off = 0;
    fp->readable = 1;
    fp->writable = 0;

    return fp;
}

void fs_test_file(void)
{
    printk("fs: file test start\n");

    struct file *f = file_open("/hello", O_RDONLY);
    if (f == NULL) 
    {
        printk("fs: file test failed: file_open\n");
        return;
    }

    char buf[64];
    int n = file_read(f, buf, sizeof(buf) - 1);
    if (n < 0) 
    {
        printk("fs: file test failed: file_read\n");
        return;
    }
    buf[n] = '\0';

    printk("fs: file /hello content: \"%s\"\n", buf);

    if (strcmp(buf, "Hello from inode!\n") != 0) 
    {
        printk("fs: file test failed: content mismatch\n");
        return;
    }

    file_close(f);

    printk("fs: file test [OK]\n");
}