#pragma once
#include "common.h"

#define PROC_NAME_MAX 16 // プロセス名の最大長
#define PROC_MAX      64 // 最大プロセス数

struct context
{
    uint32_t ra;    // return address
    uint32_t sp;    // stack pointer
    uint32_t s0;    // callee-saved register
    uint32_t s1;    // callee-saved register
    uint32_t s2;    // callee-saved register
    uint32_t s3;    // callee-saved register
    uint32_t s4;    // callee-saved register
    uint32_t s5;    // callee-saved register
    uint32_t s6;    // callee-saved register
    uint32_t s7;    // callee-saved register
    uint32_t s8;    // callee-saved register
    uint32_t s9;    // callee-saved register
    uint32_t s10;   // callee-saved register
    uint32_t s11;   // callee-saved register
};

struct cpu
{
    struct proc* current_proc;  // 現在実行中のプロセス
    struct context context;     // CPUコンテキスト
    uint32_t cpu_id;            // CPUのID
    uint32_t noff;              // 割り込み無効化のネストカウント
    uint32_t intena;            // 割り込み有効化前の状態
};

enum proc_state 
{
    PROC_STATE_UNUSED = 0,
    PROC_STATE_USED,
    PROC_STATE_SLEEPING,
    PROC_STATE_RUNNABLE,
    PROC_STATE_RUNNING,
    PROC_STATE_ZOMBIE,
};

struct proc 
{
    enum proc_state state;          // プロセスの状態
    uint32_t pid;                   // プロセスID
    uint64_t timequantum;           // タイムクォンタム
    uint64_t timeout;               // タイムアウト時間
    bool destroyed;                 // プロセスが終了予定かどうか

    uint32_t kstack;                // カーネルスタックのアドレス
    char name[PROC_NAME_MAX];       // プロセス名
};

//!
//! プロセス管理の初期化
//!
void proc_init(struct proc* proc, uint32_t pid, const char* name);

//!
//! プロセスを作成する
//! @param name プロセス名
//! @param entry プロセスのエントリポイント
//! @return 作成したプロセスのポインタ
//!
struct proc* proc_create(const char* name, void (*entry)(void));

//!
//! 現在のプロセスをCPUから解放し、他のプロセスに実行を移す
//!
void yield(void);

//!
//! 現在のプロセスを取得する
//! @return 現在のプロセスのポインタ
//!
struct proc* get_current_proc(void);

//!
//! 現在のCPUを取得する
//! @return 現在のCPUのポインタ
//!
struct cpu* mycpu(void);