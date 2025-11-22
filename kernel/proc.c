#include "proc.h"
#include "string.h"
#include "panic.h"

struct proc proc_table[PROC_MAX];

struct proc* proc_create(const char* name, void (*entry)(void)) 
{
    for (int i = 0; i < PROC_MAX; i++) 
    {
        if (proc_table[i].state == PROC_UNUSED) 
        {
            struct proc* p = &proc_table[i];
            p->pid = i;
            p->state = PROC_RUNNABLE;

            // スタックの初期化
            memset(p->stack, 0, sizeof(p->stack));
            p->sp = (vaddr32_t)(p->stack + sizeof(p->stack));

            // エントリポイントの設定
            uint32_t* sp = (uint32_t*)p->sp;
            *(--sp) = (uint32_t)entry; // リターンアドレスにエントリポイントを設定
            p->sp = (vaddr32_t)sp;

            return p;
        }
    }
    panic("proc_create: no available process slots"); // プロセススロットがない場合はパニック
}