#include "spinlock.h"
#include "interrupt.h"
#include "panic.h"

void spinlock_init(struct spinlock *lk, const char *name) 
{
    lk->locked = 0;
    lk->name = name;
    lk->cpu = 0;
}

void spinlock_acquire(struct spinlock *lk) 
{
    push_off(); // 割り込みを無効化
    if (spinlock_holding(lk)) 
    {
        // すでにロックを保持している場合はエラー
        panic("spinlock_acquire: already holding lock");
    }

    // riscvのアトミック命令を使用してロックを取得
    // 0から1に変更できたらロック取得成功
    while (__sync_lock_test_and_set(&lk->locked, 1) != 0) 
    {
        // スピン待ち
    }

    __sync_synchronize(); // メモリバリア

    // ロックを取得したCPUのIDを保存
    lk->cpu = mycpu();
}

void spinlock_release(struct spinlock *lk) 
{
    if (!spinlock_holding(lk)) 
    {
        // ロックを保持していない場合はエラー
        panic("spinlock_release: not holding lock");
    }

    lk->cpu = 0;

    __sync_synchronize(); // メモリバリア

    // ロックを解放
    __sync_lock_release(&lk->locked);

    pop_off(); // 割り込みを元に戻す
}

bool spinlock_holding(struct spinlock *lk) 
{
    return lk->locked && lk->cpu == mycpu();
}

void push_off(void) 
{
    uint32_t old = intr_get(); // 現在の割り込み状態を取得
    intr_off(); // 割り込みを無効化

    struct cpu *c = mycpu();
    if (c->noff == 0) 
    {
        c->intena = old; // 最初のネストなら元の状態を保存
    }
    c->noff++; // ネストカウントを増加
}

void pop_off(void) 
{
    struct cpu *c = mycpu();
    if (intr_get()) 
    {
        panic("pop_off: interruptible");
    }
    if (c->noff < 1) 
    {
        panic("pop_off: no matching push_off");
    }
    c->noff--; // ネストカウントを減少
    if (c->noff == 0 && c->intena) 
    {
        intr_on(); // ネストが0なら元の割り込み状態に戻す
    }
}