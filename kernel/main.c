#include "common.h"
#include "string.h"
#include "printk.h"
#include "panic.h"
#include "csr.h"
#include "trap.h"
#include "alloc.h"
#include "proc.h"
#include "swich.h"
#include "console.h"
#include "proc.h"

extern char __bss[];
extern char __bss_end[];

static void kernel_main(void);

void start_kernel(void) 
{
    // bssセグメントのゼロ初期化
    size_t bss_size = (size_t)(__bss_end - __bss);
    memset(__bss, 0, bss_size);

    // 例外ハンドラの初期化
    trap_init();

    // カーネルメイン関数の呼び出し
    kernel_main();
}

static void worker(void *arg) 
{
    uint32_t id = (uint32_t)(uintptr_t)arg;
    while (1) 
    {
        printk("worker id:%d runnning!\n", id);
        yield();
    }
}

static void kernel_main(void) 
{
    printk("Kernel initialized successfully.\n");
    printk("Hello, KINAKO OS!\n");

    // プロセス管理の初期化
    proc_init();

    // ワーカープロセスの作成
    proc_create(worker, (void*)(uintptr_t)0, "worker0");
    proc_create(worker, (void*)(uintptr_t)1, "worker1");

    // スケジューラの開始
    scheduler_start();

    while (1) 
    {
        __asm__ volatile ("wfi");
    }
}