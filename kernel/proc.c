#include "proc.h"
#include "string.h"
#include "panic.h"
#include "swich.h"
#include "kalloc.h"
#include "alloc.h"
#include "MMU.h"

/* マルチコア対応へ向けたCPU情報の取得など */
struct cpu cpus[CPU_MAX];       // 全てのCPU

struct cpu* mycpu(void) 
{
    uint32_t cpu_id;
    __asm__ volatile ("mv %0, tp" : "=r"(cpu_id));
    extern struct cpu cpus[];
    return &cpus[cpu_id];
}

/* グローバル変数群 */
struct proc g_procs[PROC_MAX];                      // 全てのプロセス
struct proc *g_current_proc = (struct proc*)NULL;   // 現在実行中のプロセス
extern char __kernel_base[];
extern char __free_ram_end[];

/* 内部で用いるグローバル変数群 */
static struct context g_boot_context;               // ブートプロセスのコンテキスト
static uint32_t next_pid = 1;                       // 次に割り当てるプロセスID

//!
//! 新しいプロセスIDを割り当てる
//!
static uint32_t alloc_pid(void) 
{
    return next_pid++;
}

/* 内部で用いる関数群 */

//!
//! 新しいプロセス構造体を割り当てる
//! @return 成功した場合はプロセス構造体へのポインタ、
//!         割り当て可能なプロセス構造体が存在しない場合はNULLを返す
//!
static struct proc* __alloc_proc();

//!
//! プロセスコンテキストを初期化する
//! @param p     初期化するプロセス構造体へのポインタ
//! @param entry プロセスのエントリポイント関数
//! @param arg   プロセスのエントリポイント関数に渡す引数
//!
static void __init_proc_context(struct proc* p, proc_entry_t entry, void* arg);

//!
//! 次にスケジューリングするプロセスを選択する
//! @return スケジューリング対象のプロセス構造体へのポインタ
//! @note ラウンドロビンスケジューリングを採用している
//!
static struct proc* __pickup_next_proc();

//!
//! スケジューリングを実行する内部関数
//!
static void __schedule();

//!
//! プロセスのトランポリン関数
//! @note 新規プロセスが最初にschedulerから帰ってきた時に呼び出される
//!
static void __proc_trampoline();


static struct proc* __alloc_proc() 
{
    for (uint32_t i = 0; i < PROC_MAX; i++) 
    {
        struct proc* p = &g_procs[i];
        if (p->state == PROC_STATE_UNUSED) 
        {
            // proc構造体を初期化
            memset(p, 0, sizeof(*p));
            p->state = PROC_STATE_READY;
            p->pid = alloc_pid();
            return p;
        }
    }
    return (struct proc*)NULL; // 割り当て可能なプロセス構造体が存在しない
}

static struct proc* __pickup_next_proc() 
{
    if (!g_current_proc) 
    {
        // 現在のプロセスが存在しない場合は先頭から探索
        for (uint32_t i = 0; i < PROC_MAX; i++)
        {
            struct proc* p = &g_procs[i];
            if (p->state == PROC_STATE_READY) 
            {
                return p;
            }

            return (struct proc*)NULL; // 実行可能なプロセスが存在しない
        }
    }

    // 現在のプロセス番号を特定
    uint32_t current_idx = -1;
    for (uint32_t i = 0; i < PROC_MAX; i++)
    {
        if (&g_procs[i] == g_current_proc)
        {
            current_idx = i;
            break;
        }
    }

    if (current_idx < 0) current_idx = 0;

    // 次のプロセスを探索
    for (uint32_t i = 1; i <= PROC_MAX; i++) 
    {
        uint32_t idx = (current_idx + i) % PROC_MAX; // 次のプロセスを循環的に探索
        struct proc* p = &g_procs[idx];
        if (p->state == PROC_STATE_READY) 
        {
            // 実行可能なプロセスが発見されたらそれを返す
            return p;
        }
    }

    return (struct proc*)NULL; // 実行可能なプロセスが存在しない
}

static void __schedule() 
{
    struct proc *current_proc = g_current_proc;
    struct proc *next_proc = __pickup_next_proc();

    if (!next_proc) 
    {
        if (current_proc && current_proc->state == PROC_STATE_RUNNING) 
        {
            // 現在のプロセスが実行中であれば、そのまま継続実行
            return;
        } 
        else 
        {
            // 実行可能なプロセスが存在しない場合はパニック
            panic("scheduler: no runnable process found");
        }
    }

    if (current_proc == next_proc) 
    {
        // 現在のプロセスと次のプロセスが同じ場合は何もしない
        return;
    }

    // プロセス状態の更新
    struct proc *prev_proc = current_proc;
    g_current_proc = next_proc;

    next_proc->state = PROC_STATE_RUNNING;
    
    // ページテーブルの切り替えをできるようにする
    __asm__ __volatile__
    (
        "sfence.vma\n"
        "csrw satp, %[satp]\n"
        "sfence.vma\n"
        :
        // 行末のカンマを忘れずに！
        : [satp] "r" (SATP_SV32 | ((uint32_t) next_proc->pagetable / PAGE_SIZE))
    );

    // カーネルスタックのアドレスをsscrathに退避させる。
    __asm__ volatile ("csrw sscratch, %0" :: "r"(next_proc->kstack_top));

    // コンテキストスイッチを実行
    if (prev_proc)
    {
        prev_proc->state = PROC_STATE_READY;
        context_switch(&prev_proc->context, &next_proc->context);
    } 
    else 
    {
        context_switch(&g_boot_context, &next_proc->context);
    }
}

/* 公開API群 */
void proc_init(void) 
{
    // 全プロセス構造体を初期化
    memset(g_procs, 0, sizeof(g_procs));

    // プロセス構造体の状態を全て未使用に設定
    for (uint32_t i = 0; i < PROC_MAX; i++) 
    {
        g_procs[i].state = PROC_STATE_UNUSED;
        g_procs[i].pid = 0;
    }
    g_current_proc = (struct proc*)NULL;
}

uint32_t proc_create(proc_entry_t entry, void* arg, const char* name) 
{
    struct proc* p = __alloc_proc();
    if (!p)
    {
        panic("proc_create: no available proc structures");
    }

    // カーネルスタックを割り当てる
    void* kstack = kalloc_page();
    if (!kstack)
    {
        // カーネルスタックの割り当てに失敗
        // プロセス構造体を未使用に戻してからエラーとする
        p->state = PROC_STATE_UNUSED;
        p->pid = 0;
        panic("proc_create: kernel stack allocate missed. \n");
    }

    p->kstack_base = kstack;
    p->kstack_top = (uint8_t *)kstack + KSTACK_SIZE;

    // カーネルのページをマッピングする
    uint32_t *pagetable = (uint32_t *)alloc_pages(1);
    kernel_map_page(pagetable);

    p->pagetable = pagetable;

    // プロセスコンテキストを初期化
    __init_proc_context(p, entry, arg);

    // プロセス名を設定
    if (name) 
    {
        strncpy(p->name, name, PROC_NAME_MAX - 1);
        p->name[PROC_NAME_MAX - 1] = '\0'; // 念のためヌル終端
    }

    printk("proc_create: done. proc_name=%s pid=%d\n", p->name, p->pid);
    return p->pid;
}

void scheduler_start(void)
{
    // 最初のスケジューリング対象プロセスを探す
    struct proc* next_proc = (struct proc*)NULL;
    for (uint32_t i = 0; i < PROC_MAX; i++)
    {
        struct proc* p = &g_procs[i];
        if (p->state == PROC_STATE_READY) 
        {
            next_proc = p;
            break;
        }
    }

    // スケジューリング対象プロセスが見つからなかった場合はパニック
    if (!next_proc)
    {
        panic("scheduler_start: no runnable process found");
    }

    // 現在のプロセスを設定
    g_current_proc = next_proc;
    next_proc->state = PROC_STATE_RUNNING;

    // コンテキストスイッチを実行
    context_switch(&g_boot_context, &next_proc->context);

    // ここには戻らない
    panic("scheduler_start: returned from context_switch unexpectedly");
}

void yield(void) 
{
    struct proc* current_proc = g_current_proc;

    // 現在のプロセスが存在しない場合はパニック
    if (!current_proc)
    {
        panic("yeild: no current process");
    }

    current_proc->state = PROC_STATE_READY;
    __schedule();
}

void schedule_tick(void) 
{
    yield();
}

void proc_exit(void)
{
    struct proc *current_proc = g_current_proc;

    // 現在のプロセスが存在しない場合はパニック
    if (!current_proc) 
    {
        panic("proc_exit: no current process");
    }

    // カーネルスタックを解放
    if (current_proc->kstack_base)
    {
        kfree_page(current_proc->kstack_base);
        current_proc->kstack_base = NULL;
        current_proc->kstack_top = NULL;
    }

    // プロセス状態を未使用に設定
    current_proc->state = PROC_STATE_READY;
    current_proc->pid = 0;
    
    // スケジューリングを実行して他のプロセスに切り替え
    g_current_proc = (struct proc*) NULL;

    __schedule();

    // ここには戻らない
    panic("proc_exit: returned from context_switch unexpectedly");
}

/* 初期化コンテキストおよびトランポリン */

// 新規プロセスが最初にschedulerから帰ってきた時に
// ra = proc_trampolineとなるようにセットしておいて、
// そこでentry(arg)を呼び出すようにする。
static void __init_proc_context(struct proc* p, proc_entry_t entry, void* arg) 
{
    // コンテキストをゼロクリア
    memset(&p->context, 0, sizeof(struct context));

    // スタックポインタをカーネルスタックのトップに設定
    uint32_t sp = (uint32_t)p->kstack_top;
    p->context.sp = sp;

    // 戻り先アドレスにプロセスのエントリポイントを設定
    p->context.ra = (uint32_t)__proc_trampoline;
    
    // 引数はレジスタ経由で渡すため、s0に設定（RISC-Vの呼び出し規約に準拠）
    p->context.s0 = (uint32_t)entry;
    p->context.s1 = (uint32_t)arg;
}

static void __proc_trampoline() 
{
    proc_entry_t entry;
    void *arg;

    // プロセスのエントリポイント関数と引数をレジスタから取得
    __asm__ volatile (
        "mv %0, s0\n"   // エントリポイント関数をs0から取得
        "mv %1, s1\n"   // 引数をs1から取得
        : "=r"(entry), "=r"(arg)
    );

    entry(arg);

    // プロセスが終了したらproc_exitを呼び出す
    proc_exit();
}