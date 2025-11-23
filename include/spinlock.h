#pragma once
#include "common.h"
#include "proc.h"

struct spinlock 
{
    uint32_t locked;    // ロック状態 (0: 解放, 1: ロック中)
    const char *name;   // ロックの名前 (デバッグ用)
    struct cpu *cpu;    // ロックを保持しているCPUのポインタ
};

//!
//! スピンロックを初期化する
//! @param lk 初期化するスピンロックのポインタ
//! @param name スピンロックの名前
//!
void spinlock_init(struct spinlock *lk, const char *name);

//!
//! スピンロックを取得する
//! @param lk 取得するスピンロックのポインタ
//!
void spinlock_acquire(struct spinlock *lk);

//!
//! スピンロックを解放する
//! @param lk 解放するスピンロックのポインタ
//!
void spinlock_release(struct spinlock *lk);

//!
//! スピンロックが現在取得されているかどうかを確認する
//! @param lk 確認するスピンロックのポインタ
//! @return 取得されていれば1、そうでなければ0
//!
bool spinlock_holding(struct spinlock *lk);

//!
//! 割り込みを無効化し、ネストカウントを増加させる
//!
void push_off(void);

//!
//! 割り込みを有効化し、ネストカウントを減少
//!
void pop_off(void);