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

void delay(void) {
    for (int i = 0; i < 30000000; i++)
        __asm__ __volatile__("nop"); // 何もしない命令
}

struct proc *proc_a;
struct proc *proc_b;

void proc_a_entry(void) {
    printk("starting process A\n");
    while (1) {
        console_putc('A');
        yield();
        delay();
    }
}

void proc_b_entry(void) {
    printk("starting process B\n");
    while (1) {
        console_putc('B');
        yield();
        delay();
    }
}

static void kernel_main(void) 
{
    printk("Kernel initialized successfully.\n");
    printk("Hello, KINAKO OS!\n");

    struct proc* idle_proc = proc_create("idle", NULL);
    idle_proc->pid = 0; // PID 0をアイドルプロセスに割り当てる
    struct proc* current_proc = idle_proc;

    proc_a = proc_create("proc_a", proc_a_entry);
    proc_b = proc_create("proc_b", proc_b_entry);
    
    yield(); // 最初のプロセスに切り替え

    panic("kernel_main: should not reach here");

    while (1) 
    {
        __asm__ volatile ("wfi");
    }
}