#include "proc.h"
#include "string.h"
#include "panic.h"
#include "swich.h"

struct proc procs[PROC_MAX];   // 全てのプロセス

void proc_init(struct proc* proc, uint32_t pid, const char* name) 
{
    proc->state = PROC_STATE_USED;
    proc->pid = pid;
    proc->kstack = 0; // カーネルスタックの初期化は別途行う
    strcpy(proc->name, name);
}
