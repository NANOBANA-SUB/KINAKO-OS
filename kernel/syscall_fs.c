#include "syscall_fs.h"
#include "proc.h"
#include "fs.h"

extern struct proc *g_current_proc;     // 現在動いているproc

/* ==== 内部向けヘルパー関数など ========================================== */
//!
//! プロセスに対してFileDescripterをひとつ割り当てる内部向けヘルパー関数
//! @param p 割り当てを行いたいプロセス
//! @param f 割り当てをするファイル
//! @retrun 割り当てされた際のファイルディスクリプタ(int)
//!
static int __fdalloc(struct proc *p, struct file *f)
{
    for (int fd = 0; fd < NOFILE; fd++)
    {
        if (p->ofile[fd] == NULL)
        {
            p->ofile[fd] = f;
            return fd;
        }
    }
    return -1;
}

/* ==== 実装 =========================================================== */
#include "printk.h"
static inline uint32_t r_sstatus(void)
{
    uint32_t x;
    asm volatile("csrr %0, sstatus" : "=r"(x));
    return x;
}

static inline void w_sstatus(uint32_t x)
{
    asm volatile("csrw sstatus, %0" :: "r"(x));
}

static inline uint32_t set_SUM(void)
{
    uint32_t old = r_sstatus();
    uint32_t new = old | (1 << 18);      // bit18 = SUM
    w_sstatus(new);
    return old;
}

static inline void restore_sstatus(uint32_t old)
{
    w_sstatus(old);
}

int copyinstr(char *dst, const char *usrc, int max)
{
    uint32_t old = set_SUM();  // 一時的に U ページを読めるようにする

    int i;
    for (i = 0; i < max - 1; i++) 
    {
        char c = usrc[i];      // ここでユーザメモリ読み
        dst[i] = c;
        if (c == '\0')
            break;
    }
    dst[i] = '\0';

    restore_sstatus(old);
    return 0;
}

// ユーザ空間の buf に len バイト書き込む
int copyout(void *user_dst, const void *kernel_src, int len)
{
    uint32_t old = set_SUM();  // S から U に書き込めるようにする

    char *ud = (char *)user_dst;
    const char *ks = (const char *)kernel_src;
    for (int i = 0; i < len; i++) 
    {
        ud[i] = ks[i];
    }

    restore_sstatus(old);
    return 0;
}

int sys_open(const char *user_path, int omode)
{
    char kpath[128];

    if (copyinstr(kpath, user_path, sizeof(kpath)) < 0) 
    {
        return -1;
    }

    printk("sys_open: path=%s\n", kpath);

    struct file *f = file_open(kpath, omode);
    if (f == NULL)
    {
        return -1;
    }

    int fd = __fdalloc(g_current_proc, f);
    if (fd < 0)
    {
        file_close(f);
        return -1;
    }
    printk("sys_open: finished fd:%d\n", fd);
    return fd;
}

int sys_read(int fd, void *user_buf, uint32_t n)
{
    printk("sys_read: fd=%d, buf=%p, n=%d\n", fd, user_buf, n);
    struct proc *p = g_current_proc;

    if (fd < 0 || fd >= NOFILE)
    {
        printk("sys_read: fd illegal\n");
        return -1;
    }

    struct file *f = p->ofile[fd];
    if (f == NULL)
    {
        printk("sys_read: f NULL\n");
        return -1;
    }

    // カーネルバッファに読み込む
    char kbuf[512];
    if (n > (int)sizeof(kbuf))
    {
        n = sizeof(kbuf);
    }

    int r = file_read(f, kbuf, n);
    if (r < 0)
    {
        printk("sys_read: file_read r < 0\n");
        return -1;
    }

    // 読み込んだ分だけユーザバッファにコピーする
    if (r > 0)
    {
        if (copyout(user_buf, kbuf, r) < 0)
        {
            printk("sys_read: copyout failed\n");
            return -1;
        }
    }

    printk("sys_read:finished\n");
    return r;
}

int sys_close(int fd)
{
    struct proc *p = g_current_proc;

    if (fd < 0 || fd >= NOFILE)
    {
        return -1;
    }

    struct file *f = p->ofile[fd];
    if (f == NULL)
    {
        return -1;
    }

    p->ofile[fd] = NULL;
    file_close(f);

    return 0;
}