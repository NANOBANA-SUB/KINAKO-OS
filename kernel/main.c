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
#include "fs.h"

#define SSTATUS_SPIE (1 << 5)

extern char __bss[];
extern char __bss_end[];

static void kernel_main(void);

static void print_kinakoOS(void)
{
    // 起動時のAA

/**
 *    [..   [..  [..[...     [..      [.       [..   [..      [....              [....       [.. ..  
 *    [..  [..   [..[. [..   [..     [. ..     [..  [..     [..    [..         [..    [..  [..    [..
 *    [.. [..    [..[.. [..  [..    [.  [..    [.. [..    [..        [..     [..        [.. [..      
 *    [. [.      [..[..  [.. [..   [..   [..   [. [.      [..        [..     [..        [..   [..    
 *    [..  [..   [..[..   [. [..  [...... [..  [..  [..   [..        [..     [..        [..      [.. 
 *    [..   [..  [..[..    [. .. [..       [.. [..   [..    [..     [..        [..     [.. [..    [..
 *    [..     [..[..[..      [..[..         [..[..     [..    [....              [....       [.. ..  
 *                                                                                                   
 */
    printk("\n");
    printk("   [..   [..  [..[...     [..      [.       [..   [..      [....              [....       [.. .. \n");
    printk("   [..  [..   [..[. [..   [..     [. ..     [..  [..     [..    [..         [..    [..  [..    [..\n");
    printk("   [.. [..    [..[.. [..  [..    [.  [..    [.. [..    [..        [..     [..        [.. [..      \n");
    printk("   [. [.      [..[..  [.. [..   [..   [..   [. [.      [..        [..     [..        [..   [..    \n");
    printk("   [..  [..   [..[..   [. [..  [...... [..  [..  [..   [..        [..     [..        [..      [.. \n");
    printk("   [..   [..  [..[..    [. .. [..       [.. [..   [..    [..     [..        [..     [.. [..    [..\n");
    printk("   [..     [..[..[..      [..[..         [..[..     [..    [....              [....       [.. ..  \n");
    printk("\n");
}
void start_kernel(void) 
{
    // bssセグメントのゼロ初期化
    size_t bss_size = (size_t)(__bss_end - __bss);
    memset(__bss, 0, bss_size);

    print_kinakoOS();
    printk("Kernel Initialize Start.\n");

    // 例外ハンドラの初期化
    trap_init();

    // ファイルシステムの初期化
    fs_init();

    // カーネルメイン関数の呼び出し
    kernel_main();
}

static void worker(void *arg) 
{
    uint32_t id = (uint32_t)(uintptr_t)arg;
    for(uint32_t i = 0; i < 5; i++)
    {
        printk("worker id:%d runnning!\n", id);
        yield();
    }
}

__attribute__((naked)) void user_entry(void *arg) {
    __asm__ __volatile__(
        "csrw sepc, %[sepc]\n"
        "csrw sstatus, %[sstatus]\n"
        "sret\n"
        :
        : [sepc] "r" (USER_BASE),
          [sstatus] "r" (SSTATUS_SPIE)
    );
}

static void kernel_main(void) 
{
    printk("Kernel initialized successfully.\n");
    printk("Hello, KINAKO OS!\n");

    // プロセス管理の初期化
    proc_init();

    // ワーカープロセスの作成
    // proc_create(worker, (void*)(uintptr_t)0, "worker0");
    // proc_create(worker, (void*)(uintptr_t)1, "worker1");

    proc_create(user_entry, (void*)(uintptr_t)1, "shell");

    // スケジューラの開始
    scheduler_start();

    while (1) 
    {
        __asm__ volatile ("wfi");
    }
}