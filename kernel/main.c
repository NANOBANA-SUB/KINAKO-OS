#include "common.h"
#include "string.h"
#include "printk.h"
#include "panic.h"
#include "csr.h"
#include "trap.h"

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

static void kernel_main(void) 
{
    printk("Kernel initialized successfully.\n");
    printk("Hello, KINAKO OS!\n");

    while (1) 
    {
        __asm__ volatile ("wfi");
    }
}