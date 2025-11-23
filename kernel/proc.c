#include "proc.h"
#include "string.h"
#include "panic.h"
#include "swich.h"
#include "spinlock.h"


/* マルチコア対応へ向けたCPU情報の取得など */
struct cpu cpus[CPU_MAX];       // 全てのCPU

struct cpu* mycpu(void) 
{
    uint32_t cpu_id;
    __asm__ volatile ("mv %0, tp" : "=r"(cpu_id));
    extern struct cpu cpus[];
    return &cpus[cpu_id];
}

/* グローバル情報 */
struct proc g_procs[PROC_MAX];                      // 全てのプロセス
struct proc *g_current_proc = (struct proc*)NULL;   // 現在実行中のプロセス

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
static struct proc* alloc_proc();

static struct proc* alloc_proc() 
{
    for (uint32_t i = 0; i < PROC_MAX; i++) 
    {
        struct proc* p = &g_procs[i];
        if (p->state == PROC_STATE_UNUSED) 
        {
            // proc構造体を初期化
            memset(p, 0, sizeof(*p));
            p->state = PROC_STATE_USED;
            p->pid = alloc_pid();
            return p;
        }
    }
    return (struct proc*)NULL; // 割り当て可能なプロセス構造体が存在しない
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
    struct proc* p = alloc_proc();
    if (!p)
    {
        panic("proc_create: no available proc structures");
    }
}